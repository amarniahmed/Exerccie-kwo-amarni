<?php

/***

https://developers.google.com/oauthplayground

https://code.google.com/apis/console

revoke access
https://accounts.google.com/b/0/IssuedAuthSubTokens?hl=en

Pb: invalid_grant
- générer un nouveau "Client ID for web applications"
  (il doit être de la forme xxxxxxxxxxxx-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.apps.googleusercontent.com)
- vérifier l'heure du server


***/


// https://developers.google.com/accounts/docs/OAuth2WebServer

// https://developers.google.com/blogger/docs/3.0/using#AboutAuthorization
// https://developers.google.com/accounts/docs/OAuth2WebServer

class GoogleClient {

  private $access_token = null;

  private $opts = null;
  private $format = 'json';
  private $oauth = null;
  private static $oauth_args = array('oauth_callback',
                                     'oauth_token',
                                     'oauth_verifier',
                                     'oauth_token_secret');

  protected $blogger_api_endpoint = 'https://www.googleapis.com/blogger/v3/';

  const USER_INFO_URL = 'https://www.googleapis.com/oauth2/v1/userinfo';

  const HOST = 'https://www.google.com/accounts';
  const REQUEST_TOKEN_URL = 'https://www.google.com/accounts/OAuthGetRequestToken';
  const ACCESS_TOKEN_URL = 'https://www.google.com/accounts/OAuthGetAccessToken';

  // v1
  const AUTHORIZE_URL = 'https://accounts.google.com/OAuthAuthorizeToken';

  // v2
  const AUTHORIZATION_ENDPOINT = 'https://accounts.google.com/o/oauth2/auth';
  const TOKEN_ENDPOINT         = 'https://accounts.google.com/o/oauth2/token';

  public function __construct($opts=null) {
    $opts = (array) $opts;
    $this->oauth = array_merge(array('version' => '1.0',
                                     'signature_method' => 'HMAC-SHA1'),
                               $opts);
  }

  public function getRequestToken($parameters, $opts=null) {
    foreach (array('oauth_callback', 'scope') as $f) {
      if (!$parameters[$f]) {
        S('log')->trace('missing "'.$f.'" in request token ', __METHOD__);
        return false;
      }
    }
    $resp = $this->fetch('GET', self::REQUEST_TOKEN_URL, $parameters, $opts);
    parse_str($resp, $h);
    return $h;
  }

  /**
     Blogger : https://www.googleapis.com/auth/blogger
     Project id : 82964214151
   **/
  public function getAuthorizationUrl($oauth_token, $opts=null) {
    if (is_array($oauth_token) && $oauth_token['client_id']) {
      $params = $oauth_token;
      $scope = is_array($params['scope']) ? join(' ', $params['scope']) : $params['scope'];
      $parameters = array('scope' => $scope,
                          'access_type' => $params['access_type'] ?: 'offline',
                          'approval_prompt' => $params['approval_prompt'] ?: 'auto',
                          'client_id' => $params['client_id'],
                          'redirect_uri' => $params['redirect_uri'],
                          'response_type' => 'code',
                          'state' => 'google');
      $url = H::url(self::AUTHORIZATION_ENDPOINT, $parameters);
      return $url;
    }
    $opts = (array) $opts;
    $params = array('oauth_token' => $oauth_token);
    if ($opts['callback_url']) {
      $params['oauth_callback'] = $opts['callback_url'];
    }
    return H::url(self::AUTHORIZE_URL, $params);
  }

  public function setAccessToken($access_token) {
    $this->access_token = $access_token;
  }

  public function refreshAccessToken() {

  }

  public function getAccessToken($params, $opts=null) {
    if ($params['client_id']) {
      // oauthv2 : exchange "authorization code" (code) for an "access token" (+ "refresh token")
      $parameters = array('code' => $params['code'],
                          'client_id' => $params['client_id'],
                          'client_secret' => $params['client_secret'],
                          'redirect_uri' => $params['redirect_uri'],
                          'grant_type' => $params['grant_type'] ?: 'authorization_code');
      S('log')->debug($parameters);
      //    $resp = $this->fetch('POST', 'https://accounts.google.com/o/oauth2/token', $parameters, $opts);
      $opts = array('debug' => false);
      $resp = HttpClient::post(self::TOKEN_ENDPOINT, $parameters, $opts);
      $h = json_decode($resp, true);
      //    parse_str($resp, $h);
      return $h;
    }
    foreach (array('oauth_token', 'oauth_token_secret') as $f) {
      if (!$params[$f]) {
        S('log')->trace('missing "'.$f.'" in access token ', __METHOD__);
        return false;
      }
    }
    $resp = $this->fetch('GET', self::ACCESS_TOKEN_URL, $params, $opts);
    parse_str($resp, $h);
    return $h;
  }

  public function fetch($method, $url, $parameters, $opts=null) {
    $opts = (array) $opts;
    $url = stripos($url, 'http') !== 0 ? self::HOST.$url : $url;
    $ts = time();
    $oauth = array('oauth_version' => $this->oauth['version'],
                   'oauth_nonce' => md5($ts),
                   'oauth_timestamp' => $ts,
                   'oauth_consumer_key' => $this->oauth['consumer_key'],
                   'oauth_signature_method' => $this->oauth['signature_method']);
    foreach (self::$oauth_args as $oauth_field) {
      if (isset($parameters[$oauth_field])) {
        $this->oauth[$oauth_field] = $parameters[$oauth_field];
        $oauth[$oauth_field] = $parameters[$oauth_field];
        unset($parameters[$oauth_field]);
      }
    }
    $args = array_merge($oauth, $parameters);
    $this->oauth['signature'] = $this->getSignature($method, $url, $args);
    $opts['method'] = $method;
    $opts['headers'] = array('Authorization' => $this->getAuthorizationHeader($args));
    $resp = HttpClient::call($url, $parameters, $opts);
    return $resp;
  }

  public function getAuthorizationHeader($args) {
    $parts = array('oauth_version' => $this->oauth['version'],
                   'oauth_nonce' => $args['oauth_nonce'],
                   'oauth_timestamp' => $args['oauth_timestamp'],
                   'oauth_consumer_key' => $this->oauth['consumer_key'],
                   'oauth_signature_method' => $this->oauth['signature_method']);
    foreach (self::$oauth_args as $oauth_field) {
      if (isset($args[$oauth_field])) {
        $parts[$oauth_field] = rawurlencode($args[$oauth_field]);
      }
    }
    $parts['oauth_signature'] = $this->oauth['signature'];
    foreach ($parts as $key => &$value) {
      $value = $key.'='.'"'.$value.'"';
    }
    $header = 'OAuth '.join(', ', $parts);
    return $header;
  }

  public function getSignature($method, $url, $parameters) {
    $signature = hash_hmac('SHA1',
                           $this->getBaseString($method, $url, $parameters),
                           $this->getKey($parameters),
                           true);
    $signature = base64_encode($signature);
    $signature = rawurlencode($signature);
    return $signature;
  }

  private function getBaseString($method, $url, $parameters) {
    $params = $parameters;
    if (isset($params['oauth_signature'])) {
      unset($params['oauth_signature']);
    }
    $params = OAuth::build_http_query($params);
    $parts = array(strtoupper($method), $url, $params);
    $base_string = $this->encode($parts);
    return $base_string;
  }

  private function getKey($parameters=null) {
    $parameters = (array) $parameters;
    $parts = array($this->oauth['consumer_secret'],
                   $this->oauth['oauth_token_secret'] ?: '');
    $key = $this->encode($parts);
    return $key;
  }

  private function encode($input) {
    foreach ($input as &$value) {
      $value = rawurlencode($value);
    }
    $str = implode('&', $input);
    return $str;
  }

  public static function login2() {
    $session = S('req')->getSession();
    $oauth = P('oauth.providers');
    $params = array('consumer_key' => $oauth['google']['client_id'],
                    'consumer_secret' => $oauth['google']['client_secret']);
    $client = GoogleClient::getInstance($params);
    $scopes = array('https://www.googleapis.com/auth/userinfo.profile',
                    'https://www.googleapis.com/auth/userinfo.email');
    $opts = array('scope' => join(' ', $scopes),
                  'oauth_callback' => H::url(OAuth::LOGIN_CALLBACK_URL.'?state=google'));
    $h = $client->getRequestToken($opts);
    $session->setAttribute('gg_oauth_token_secret', $h['oauth_token_secret']);
    $session->commit();
    $url = H::url(self::AUTHORIZE_URL, array('oauth_token' => $h['oauth_token']));
    S('res')->sendRedirect($url);
  }

  public static function login() {
    $oauth = P('oauth.providers');
    $url = 'https://accounts.google.com/o/oauth2/auth';
    $scopes = array('https://www.googleapis.com/auth/userinfo.profile',
                    'https://www.googleapis.com/auth/userinfo.email');
    $params = array('scope' => join(' ', $scopes),
                    'client_id' => $oauth['google']['client_id'],
                    'redirect_uri' => H::url(OAuth::LOGIN_CALLBACK_URL),
                    'response_type' => 'code',
                    'state' => 'google');
    S('res')->sendRedirect(H::url($url, $params));
  }

  public static function getInstance($opts=null) {
    return new GoogleClient($opts);
  }

  //--- API Blogger

  public function getBloggerBlogs($uid) {
    $uri = 'https://www.googleapis.com/blogger/v3/users/'.$uid.'/blogs';
    return $this->callBloggerApi($uri);
  }

  public function getBloggerPosts() {

  }

  private function callBloggerApi($uri, $params=null) {
    $hreq = HttpRequest::getInstance($uri, $params, $opts);
//    $hreq->setHeader('Authorization', $this->access_token);
    $hreq->setHeader('Authorization', 'Bearer '.$this->access_token);
    $content = $hreq->send()->getContent();
//    $client->debug();
    return $content;
  }

  //--- API Contacts

  public function getContacts($parameters) {
    $url = 'https://www.google.com/m8/feeds/contacts/default/full/';
    $opts = array('headers' => array('GData-Version' => '3.0'));
    $resp = $this->fetch('GET', $url, $parameters, $opts);
    $resp = json_decode($resp, true);
    $entries = $resp['feed']['entry'];
    $contacts = array();
    if (count($entries) < 1) return $contacts;
    foreach ($entries as $entry) {
      $contacts[] = array('id' => basename($entry['id']['$t']),
                          'name' => $entry['gd$name']['gd$fullName']['$t'],
                          'first_name' => $entry['gd$name']['gd$givenName']['$t'],
                          'last_name' => $entry['gd$name']['gd$familyName']['$t'],
                          'email' => $entry['gd$email'][0]['address'],
                          'phone' => $entry['gd$phoneNumber'][0]['$t'],
                          'company' => $entry['gd$organization'][0]['gd$orgName']['$t'],
                          'updated' => $entry['updated']['$t']);
    }
    return $contacts;
  }

  public function getGroups($parameters) {
    $url = 'https://www.google.com/m8/feeds/groups/default/full';
    $opts = array('headers' => array('GData-Version' => '3.0'));
    $resp = $this->fetch('GET', $url, $parameters, $opts);
    $resp = json_decode($resp, true);
    $entries = $resp['feed']['entry'];
    $groups = array();
    if (count($entries) < 1) return $groups;
    foreach ($entries as $entry) {
      $groups[] = array('uri' => $entry['id']['$t'],
                        'name' => $entry['title']['$t'],
                        'content' => $entry['content']['$t']);
    }
    return $groups;
  }

  public function createContact($parameters) {
    $url = 'https://www.google.com/m8/feeds/contacts/default/full';
    $opts = array('headers' => array('GData-Version' => '3.0',
                                     'Content-Type' => 'application/atom+xml'));
    $resp = $this->fetch('POST', $url, $parameters, $opts);
    return $resp;
  }

  public function deleteContact($parameters) {
    $url = 'https://www.google.com/m8/feeds/contacts/default/full';
    $opts = array('headers' => array('GData-Version' => '3.0'));
    $resp = $this->fetch('DELETE', $url, $parameters, $opts);
    return $resp;
  }

  public function updateContact($parameters) {
    $url = 'https://www.google.com/m8/feeds/contacts/default/full';
    $opts = array('headers' => array('GData-Version' => '3.0',
                                     'Content-Type' => 'application/atom+xml'));
    $resp = $this->fetch('PUT', $url, $parameters, $opts);
    return $resp;
  }

  //--- API END

}