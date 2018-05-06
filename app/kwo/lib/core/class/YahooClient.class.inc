<?php

/**
https://github.com/joechung/oauth_yahoo
**/

class YahooClient
{
  const USER_INFO_URL = 'http://social.yahooapis.com/v1/user';

  private function getKey() {
    $parts = array($this->tokens['consumer_secret'],
                   $this->tokens['oauth_token_secret'] ?: '');
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

}