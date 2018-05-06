<?php

class HiconnectClient {

  private $client = null;

  const WSDL = 'http://vocalgateway.hi-media-techno.com/ws/wsdl/Hiconnect.wsdl';
  const DEFAULT_DURATION = 180;


  function __construct() {
    $credentials = P('api.credentials');
    $credentials = $credentials['hiconnect'];

//* test / P() jv1t non pris en compte dans CLI */$credentials = array('login' => 'al.balkany', 'password' => 'N774Nm6F');

    if (empty($credentials)) {
      S('log')->trace('No credentials found', __METHOD__);
      return ;
    }
    ini_set('soap.wsdl_cache_enabled', '0');
    try {
      $wsdl = $this->params['wsdl'] ?: self::WSDL;
      $options = array('login' => $credentials['login'],
                       'password' => $credentials['password'],
                       'wsdl_cache' => 0,
                       'trace' => 1);
      $this->client = new SoapClient($wsdl, $options);
    }
    catch (SoapFault $fault) {
      S('log')->error('Request failed. '.
                      '['.$fault->faultstring.'] '.
                      '('.$fault->faultcode.')', __METHOD__);
    }
  }

  function reserver_numero($destination, $opts=null) {
    $result = array();
    /*
    - destination (obligatoire)
    - duree_resa (obligatoire)
    - customer (opt)
    - ip (opt)
    - nom_cab (opt)
    - site (opt)
    */
    try {
      $duree = (int) $opts['duree_resa'] > 0 ? (int) $opts['duree_resa'] : self::DEFAULT_DURATION;
      $args = array('destination' => $destination,
                    'duree_resa' => $duree,
                    'customer' => $opts['customer'],
                    'ip' => $opts['ip'],
                    'nom_cab' => $opts['nom_cab'],
                    'site' => $opts['site']);

      $res = $this->client->__call('reserver_numero', $args);
      $result['error'] = 'OK';
      $result['infos'] = $res;
    }
    catch (SoapFault $fault) {
      $result['error'] = 'ERROR';
      $result['infos'] = 'ERROR : '.$fault->faultstring.' ('.$fault->faultcode.')';
      S('log')->error('Request failed. '.
                      '['.$fault->faultstring.'] '.
                      '('.$fault->faultcode.')', __METHOD__);
    }
    return $result;
  }

  function getLastResponse() {
  	return $this->client->__getLastResponse();
  }

  public static function getInstance() {
    return new HiconnectClient();
  }

}