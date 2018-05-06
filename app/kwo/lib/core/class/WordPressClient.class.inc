<?php

class WordPressClient
{
  public function __construct($params) {
    $this->params = $params;
  }

  public function getAccessToken() {
    $url = 'https://graph.wordpress.com/oauth/access_token';
    $params = array('client_id' => $this->params['client_id'],
                    'client_secret' => $this->params['client_secret'],
                    'redirect_uri' => $this->params['redirect_uri'],
                    'code' => $this->params['code']);
    $resp = HttpClient::get($url, $params);
    $h = null;
    parse_str($resp, $h);
    return $h;
  }

  public static function login() {
    $oauth = P('oauth.providers');
    $oauth = $oauth['wordpress'];
    $url = 'http://www.wordpress.com/dialog/oauth';
    $params = array('scope' => 'email',
                    'client_id' => $oauth['client_id'],
                    'redirect_uri' => H::url(OAuth::LOGIN_CALLBACK_URL),
                    'display' => 'popup',
                    'state' => 'wordpress');
    S('res')->sendRedirect(H::url($url, $params));
  }

  public static function getInstance($params) {
    return new WordPressClient($params);
  }

}