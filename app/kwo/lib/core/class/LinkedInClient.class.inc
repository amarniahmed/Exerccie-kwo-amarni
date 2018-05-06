<?php

class LinkedInClient {

  private $params = null;

  const USER_INFO_URL = 'https://api.linkedin.com/v1/people/~';
  const AUTHORIZATION_ENDPOINT = 'https://www.linkedin.com/uas/oauth2/authorization';
  const TOKEN_ENDPOINT = 'https://www.linkedin.com/uas/oauth2/accessToken';


  public function __construct($params) {
    $this->params = $params;
  }

  public function getAuthorizationUrl($params) {
    $scope = is_array($params['scope']) ? join(' ', $params['scope']) : $params['scope'];
    $parameters = array('response_type' => 'code',
                        'client_id' => $this->params['client_id'],
                        'scope' => $scope,
                        'state' => 'linkedin',
                        'redirect_uri' => $params['redirect_uri']);
    $url = H::url(self::AUTHORIZATION_ENDPOINT, $parameters);
    return $url;
  }

  public function getAccessToken($params, $opts=null) {
    $parameters = array('grant_type' => $params['grant_type'] ?: 'authorization_code',
                        'code' => $params['code'],
                        'redirect_uri' => $params['redirect_uri'],
                        'client_id' => $this->params['client_id'],
                        'client_secret' => $this->params['client_secret']);
    $opts = array('debug' => false);
    $resp = HttpClient::get(self::TOKEN_ENDPOINT, $parameters, $opts);
    $h = json_decode($resp, true);
    return $h;
  }

  public function fetch($method, $url, $params, $opts=null) {
    $opts = (array) $opts;
    $opts['method'] = $method;
    $resp = HttpClient::call($url, $params, $opts);
    return $resp;
  }

  public static function getInstance($params) {
    return new LinkedInClient($params);
  }

  public static function login() {
    $session = S('req')->getSession();
    $oauth = P('oauth.providers');
    $params = array('client_id' => $oauth['linkedin']['client_id'],
                    'client_secret' => $oauth['linkedin']['client_secret'],
                    'scope' => array('r_fullprofile',
                                     'r_emailaddress',
                                     'r_contactinfo',
                                     'rw_nus'),
                    'redirect_uri' => H::url(OAuth::LOGIN_CALLBACK_URL.'?state=linkedin'));
    $client = LinkedInClient::getInstance($params);
    $url = $client->getAuthorizationUrl($params);
    S('res')->sendRedirect($url);
  }

  //  API ----------------------------------------------------------------------

  public function getProfileInfos($params) {
    $selectors = array('id', 'email-address', 'date-of-birth',
                       'first-name', 'last-name', 'headline', 'location', 'picture-url',
                       'positions:(title,start-date,end-date,is-current,company:(name))',
                       'educations:(school-name,degree,start-date,end-date)');
    $url = self::USER_INFO_URL.':('.implode(',', $selectors).')';
    $resp = $this->fetch('GET', $url, $params);
    $usr = json_decode($resp, true);
    $usr['email'] = $usr['emailAddress'];
    $usr['username'] = $usr['firstName'].' '.$usr['lastName'];
    $fields = array('emailAddress', 'headline');
    foreach ($fields as $field) {
      unset($usr[$field]);
    }
    return $usr;
  }

  public function shareContent($token, $xml, $opts=null) {
    $url = self::USER_INFO_URL.'/shares?oauth2_access_token='.urlencode($token).'&format=json';
    return $this->fetch('POST', $url, $xml, $opts);
  }

}