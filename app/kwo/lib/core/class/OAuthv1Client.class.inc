<?php

// RequestToken -> AccessToken
//

class OAuthv1Client 
{
  protected $format = 'json';
  protected $oauth = null;
  protected $opts = null;
  protected $tokens = null;
    
  public function getAuthorizationUrl($oauth_token, $opts=null) {
    $opts = (array) $opts;
    $params = array('oauth_token' => $oauth_token);
    if ($opts['callback_url']) {
      $params['oauth_callback'] = $opts['callback_url'];
    }
    return H::url($this->authorize_url, $params);
  }

  public function getRequestTokens($oauth_callback) {
    $url = $this->oauth_url.'/oauth/request_token'; 
    $params = array('oauth_callback' => $oauth_callback); 
    $resp = $this->fetch('GET', $url, $params); 
    $h = array();
    parse_str($resp, $h);
    return $h;
  }
  
  public function getAccessTokens($params=null) {
    $params = (array) $params;
    if (empty($this->tokens['oauth_token']) || 
        empty($this->tokens['oauth_token_secret'])) {
      S('log')->error('invalid tokens', __METHOD__);
      return false;
    }
    $url = $this->oauth_url.'/oauth/access_token';
    $resp = $this->fetch('GET', $url, $params);
    $h = array();
    parse_str($resp, $h);
    return $h;
  }

  public function fetch($method, $url, $params=null, $opts=null) {
    $url = stripos($url, 'http') !== 0 ? $this->api_url.$url.'.'.$this->format : $url;
    $params = (array) $params;
    $authorization_header = $this->getAuthorizationHeader($method, $url, $params);
    if ($params['oauth_callback']) {
      unset($params['oauth_callback']);
    }
    $opts = array('headers' => array('Authorization' => $authorization_header));
    $response = HttpClient::post($url, $params, $opts);
    return $response;
  }
  
  public function callApi($url, $params=null, $opts=null) {
    $opts = (array) $opts;
    $method = $opts['method'] ?: 'POST';
    $params = (array) $params;
    $url = $this->api_url . $url;
    $response = $this->fetch($method, $url, $params);
//    S('log')->trace($response, __METHOD__);
    return json_decode($response, true);
  }

  public function getAuthorizationHeader($method, $url, $params) {
    $oauth = $this->oauth;
    $oauth['oauth_timestamp'] = (string) time();
    $oauth['oauth_nonce'] = md5($oauth['oauth_timestamp']);
    if ($this->tokens['oauth_token']) {
      $oauth['oauth_token'] = $this->tokens['oauth_token'];
    }
    if ($params['oauth_callback']) {
      $oauth['oauth_callback'] = $params['oauth_callback'];
      unset($params['oauth_callback']);
    }
    $oauth['oauth_signature'] = $this->getSignature($method, $url, $params, $oauth);
    ksort($oauth);
    foreach ($oauth as $key => &$value) {
      if (substr($key, 0, 6) != 'oauth_') continue ;
      $value = rawurlencode($key).'='.'"'.rawurlencode($value).'"';
    }
    $header = 'OAuth ';
    if ($this->opts['realm']) {
      $header .= 'realm="'.$this->opts['realm'].'" ';
    }
    $header .= join(', ', $oauth);
    return $header;
  }
  
  public function getTokens() {
    return $this->tokens;
  }

  public function setTokens($tokens) {
    $this->tokens = $tokens;
  }

  public function getSignature($method, $url, $params, $oauth) {
    $signature = hash_hmac('SHA1',
                           $this->getBaseString($method, $url, $params, $oauth),
                           $this->getKey(),
                           true);
    $signature = base64_encode($signature);
    return $signature;
  }

  protected function getBaseString($method, $url, $params, $oauth) {
    $attrs = array_merge($params, $oauth);
    $attrs = $this->buildHttpQuery($attrs);
    $parts = array(strtoupper($method), $url, $attrs);
    $base_string = $this->encode($parts);
    return $base_string;
  }
  
  protected function getKey() {
    $parts = array($this->tokens['consumer_secret'],
                   $this->tokens['oauth_token_secret'] ?: '');
    $key = $this->encode($parts);
    return $key;
  }

  protected function encode($input) {
    foreach ($input as &$value) {
      $value = rawurlencode($value);
    }
    $str = implode('&', $input);
    return $str;
  }

  private function buildHttpQuery($params) {
    if (!$params) return '';
    $tmp = array();
    foreach ($params as $key => $value) {
      $key = rawurlencode($key);
      $value = rawurlencode($value);
      $tmp[$key] = $value;
    }
    $params = $tmp;
    uksort($params, 'strcmp');
    $pairs = array();
    foreach ($params as $key => $value) {
      if (is_array($value)) {
        natsort($value);
        foreach ($value as $duplicate_value) {
          $pairs[] = $key . '=' . $duplicate_value;
        }
      }
      else {
        $pairs[] = $key . '=' . $value;
      }
    }
    return implode('&', $pairs);
  }
  
}