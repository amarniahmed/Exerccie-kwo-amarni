<?php

class EmailVisionClient
{
  const SYNCHRO_NOTHING = 'NOTHING';
  const SYNCHRO_INSERT = 'INSERT';
  const SYNCHRO_UPDATE = 'UPDATE';
  const SYNCHRO_INSERT_UPDATE = 'INSERT_UPDATE';

  private $login = null;
  private $password = null;
  private $api_key = null;

  private $endpoints = array();
  private $clients = array();
  private $tokens = array();

  public function __construct($login, $password, $api_key, $opts) {
    if (!is_array($opts)) {
      S('log')->error('Usage: Array<login, pwd, key, server_name>', __METHOD__);
      throw new Exception('invalid parameters');
    }
    $opts['endpoints'] = (array) $opts['endpoints'];
    foreach ($opts['endpoints'] as $api => $endpoint) {
      $this->endpoints[$api] = $endpoint;
    }
    if ($this->endpoints['nmp_trigger'] === null) {
      $this->endpoints['nmp_trigger'] = 'http://api.notificationmessaging.com';
    }
    $this->login = $login;
    $this->password = $password;
    $this->api_key = $api_key;
  }


  private function getClient($api) {
    try {
      if ($this->clients[$api] === null) {
        $pathes = array('data_mass_update' => '/apibatchmember/services/BatchMemberService?wsdl',
                        'nmp_trigger' => '/NMSOAP/NotificationService?wsdl');
        $wsdl = $this->endpoints[$api] . $pathes[$api];
        $opts = array('exceptions' => 1,
                      'wsdl_cache' => 0,
                      'trace' => 1,
                      'soap_version' => SOAP_1_1);
        $this->clients[$api] = new EmailVisionSoapClient($wsdl, $opts);
        $this->clients[$api]->api = $api;
      }
      return $this->clients[$api];
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
    }
  }

  private function call($method, $params, $client=null) {
    try {
      if ($client === null) {
        if (in_array($method, array('uploadFileInsert'))) {
          $api = 'data_mass_update';
        }
        elseif (in_array($method, array('sendObjects'))) {
          $api = 'nmp_trigger';
        }
        $client = $this->getClient($api);
      }

      if ($method != 'openApiConnection' && in_array($client->api, array('data_mass_update'))) {
        if (!isset($this->tokens[$client->api])) {
          $args = array('key' => $this->api_key,
                        'pwd' => $this->password,
                        'login' => $this->login);
          $this->tokens[$client->api] = $this->call('openApiConnection', $args, $client);
        }
        if (!isset($params['token'])) {
          $params['token'] = $this->tokens[$client->api];
        }
      }
      $response = $client->__soapCall($method, array($params));
      return $response->return;
    }
    catch (SoapFault $e) {
      S('log')->error('Soap error: '.$e->faultstring, __METHOD__);
      S('log')->error($client->__getLastRequest());
      S('log')->error($client->__getLastResponse());
      S('log')->debug($e->detail);
      throw new Exception('');
    }
  }

/*
  public function createMembers($file, $opts) {
    if (!is_a($file, 'File')) {
      throw new Exception('"file" parameter must be a CSV file');
    }

    $file_content = $file->getContent();
    $message = array('file'         => $file_content,
                     'insertUpload' => array());

    $message['insertUpload']['fileName'] = $file->getBaseName();
    $message['insertUpload']['separator'] = $opts['separator'];
    $message['insertUpload']['skipFirstLine'] = 'true';
    $message['insertUpload']['autoMapping'] = 'true';
    $message['insertUpload']['dedup']['skipUnsubAndHBQ'] = 'false';
    $message['insertUpload']['dedup']['criteria'] = 'LOWER(EMAIL)';

    $result = $this->call('uploadFileInsert', $message);

    if (!is_numeric($result)) {
      S('log')->error($result, __METHOD__);
    }
    return (int) $result; // uploadId
  }
*/
  public function createMembers($file, $opts) {
    if (!is_a($file, 'File')) {
      throw new Exception('"file" parameter must be a CSV file');
    }

    $file_content = $file->getContent();
    $message = array('file'         => $file_content,
                     'mergeUpload' => array());

    $message['mergeUpload']['fileName'] = $file->getBaseName();
    $message['mergeUpload']['separator'] = $opts['separator'];
    $message['mergeUpload']['skipFirstLine'] = 'true';
    $message['mergeUpload']['autoMapping'] = 'true';
    $message['mergeUpload']['dedup']['skipUnsubAndHBQ'] = 'false';
    $message['mergeUpload']['dedup']['criteria'] = 'LOWER(EMAIL)';

    $result = $this->call('uploadFileMerge', $message);

    if (!is_numeric($result)) {
      S('log')->error($result, __METHOD__);
    }
    return (int) $result; // uploadId
  }

  public function sendTransactionalMessage($email, $html_content, $template_id, $secure_string, $bindings) {
    $message = $bindings;
    unset($message['dyn']);

    $message['email'] = $email;
    $message['notificationId'] = $template_id;
    $message['encrypt'] = $secure_string;
    $message['senddate'] = !isset($message['senddate']) ? date('Y-m-d\TH:i:s') : date('Y-m-d\TH:i:s', strtotime($message['senddate']));
    $message['uidkey'] = 'EMAIL';

    $message['content']['entry']['key'] = 1;
    $message['content']['entry']['value'] = $html_content;
    
    $i = 0;
    foreach ($bindings['dyn'] as $k => $v) {
      $message['dyn']['entry'][$i]['key'] = $k;
      $message['dyn']['entry'][$i]['value'] = $v;
      $i++;
    }

    $message = array('arg0' => array('sendrequest' => $message));
    // S('log')->debug($message);
    $result = $this->call('sendObjects', $message);

    if ($result != 'SendRequest has been successfully saved!' || !is_object($result)) {
      S('log')->error(print_r($result, true), __METHOD__);
      return false;
    }
    return true;
  }

  public function __destruct() {
    foreach ($this->tokens as $api => $token) {
      if (empty($token)) continue ;
      $client = $this->clients[$api];
      if (empty($client)) continue ;
      $this->call('closeApiConnection', array('token' => $token), $client);
    }
  }

  private static $singleton = null;

  public static function getInstance($login, $password, $api_key, $opts=null) {
    if (self::$singleton === null) {
      $opts = (array) $opts;
      self::$singleton = new EmailVisionClient($login, $password, $api_key, $opts);
    }
    return self::$singleton;
  }
}


class EmailVisionSoapClient extends SoapClient
{
  public function __doRequest($request, $location, $action, $version, $one_way = 0) {
    $response = parent::__doRequest($request, $location, $action, $version, $one_way);
    if (strpos($response, 'application/xop+xml') === false) return $response;
    $response = $this->extractMimeBody($response);
    return $response;
  }

  private function extractMimeBody($part) {
    preg_match('/\r\n\r\n(<s.*>)\r\n-/i', $part, $matches);
    return $matches[1];
  }
}



  /***/

/*
  public function createMember($opts) {
    $member = array();
    $member_attrs = array();

    $i=0;
    foreach ($opts as $k => $v) {
      if ($k != 'email') {
        $member_attrs[$i]['key'] = $k;
        $member_attrs[$i]['value'] = $v;
        $i++;
      }
    }

    $member['dynContent']['entry'] = $member_attrs;
    $member['email'] = $opts['email'];

    $result = $this->call('insertMemberByObj', array('token'  => $this->token_member,
                                                     'member' => $member));

    if (!is_numeric($result)) {
      S('log')->error($result, __METHOD__);
    }
    return (int) $result;
  }
*/
/*
  public function updateMember($opts) {
    $member = array();
    $member_attrs = array();

    $i=0;
    foreach ($opts as $k => $v) {
      if ($k != 'email') {
        $member_attrs[$i]['key'] = $k;
        $member_attrs[$i]['value'] = $v;
        $i++;
      }
    }

    $member['dynContent']['entry'] = $member_attrs;
    $member['memberUID'] = isset($opts['email']) ? 'email:'.$opts['email'] : 'member_id:'.$opts['id'];

    $result = $this->client_member->updateMember(array( 'token' => $this->token_member,
                                                        'member' => $member))->return;
    if (!is_numeric($result)) {
      S('log')->error($result, __METHOD__);
    }
    return (int) $result;
  }
*/