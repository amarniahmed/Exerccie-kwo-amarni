<?php

class FacebookClient {

  private $params = null;

  const GRAPH_URL = 'https://graph.facebook.com';

  public function __construct($params) {
    $this->params = $params;
  }

  public function getAccessToken() {
    $url = 'https://graph.facebook.com/oauth/access_token';
    $params = array('client_id' => $this->params['client_id'],
                    'client_secret' => $this->params['client_secret'],
                    'redirect_uri' => $this->params['redirect_uri'],
                    'code' => $this->params['code']);
    $resp = HttpClient::get($url, $params);
    $h = null;
    parse_str($resp, $h);
    return $h;
  }

  public function fetch($method, $url, $params, $opts=null) {
    $opts = (array) $opts;
    $opts['method'] = $method;
    $resp = HttpClient::call($url, $params, $opts);
    return $resp;
  }

  public function getContacts($params) {
    $url = self::GRAPH_URL.'/me/friends';
    $resp = HttpClient::get($url, $params);
    $resp = json_decode($resp);
    $contacts = array();
    foreach ($resp->data as $entry) {
      $contacts[$entry->id] = $entry->name;
    }
    return $contacts;
  }

  public static function getInstance($params) {
    return new FacebookClient($params);
  }

  public static function login() {
    $oauth = P('oauth.providers');
    $url = 'http://www.facebook.com/dialog/oauth';
    $params = array('scope' => 'email',
                    'client_id' => $oauth['facebook']['client_id'],
                    'redirect_uri' => H::url(OAuth::LOGIN_CALLBACK_URL),
                    'display' => 'popup',
                    'state' => 'facebook');
    S('res')->sendRedirect(H::url($url, $params));
  }

}