<?php

/*

https://dev.twitter.com/docs/auth/creating-signature
oauth_token = a request_tokens that becomes an access_token

*/

// si pb "Read-only application cannot POST problem" : reset des tokens
// out-of-band mode OAuth

class TwitterClient extends OAuthv1Client
{
  protected $authorize_url = 'https://api.twitter.com/oauth/authorize';
  protected $api_url = 'https://api.twitter.com/1';
  protected $oauth_url = 'https://api.twitter.com';

  /** 
  tokens: consumer_key, consumer_secret [, oauth_token [, oauth_token_secret]]
  **/
  public function __construct($tokens, $opts=null) {
    if (!$tokens['consumer_key']) {
      throw new Exception('invalid tokens');
    }
    $this->opts = (array) $opts;
    $this->oauth = array('oauth_consumer_key' => $tokens['consumer_key'],
                         'oauth_nonce' => null,
                         'oauth_signature_method' => 'HMAC-SHA1',
                         'oauth_timestamp' => null,
                         'oauth_version' => '1.0');
    $this->tokens = $tokens;
  }

  public static function getInstance($tokens, $opts=null) {
    return new TwitterClient($tokens, $opts);
  }

  public static function tagify($tag) {
    $tag = SH::urlify($tag);
    $tag = str_replace('-', ' ', $tag);
    $tag = mb_convert_case($tag, MB_CASE_TITLE, 'UTF-8');
    $tag = str_replace(' ', '', $tag);
    return '#'.$tag;
  }

  public static function authorize() {
    $oauth_params = P('oauth.providers');
    $session = S('req')->getSession();
    $tokens = array('consumer_key' => $oauth_params['twitter']['consumer_key'],
                    'consumer_secret' => $oauth_params['twitter']['consumer_secret']);
    $callback_url = H::url(OAuth::LOGIN_CALLBACK_URL, array('state' => 'twitter'));
    $client = TwitterClient::getInstance($tokens);
    $h = $client->getRequestToken(array('oauth_callback' => $url));
    $session->setAttribute('tw_oauth_token_secret', $h['oauth_token_secret']);
    $session->commit();
    $url = $this->authorize_url;
    $params = array('oauth_token' => $h['oauth_token']);
    S('res')->sendRedirect(H::url($url, $params));
  }

  /*-------------------- API --------------------*/

  public function getContacts($params=null) {
    $url = '/friends/ids';
    $params = (array) $params;
    $resp = $this->fetch('GET', $url, $params);
    $resp = json_decode($resp);
    $resp = $resp->ids;
    return $resp;
  }

  public function setStatus($content) {
    $url = '/statuses/update';
    $params = array('trim_user' => 't');
    if ($this->oauth['oauth_token']) {
      $params['oauth_token'] = $this->oauth['oauth_token'];
    }
    if (!($content instanceof ActiveRecord)) {
      $params['status'] = $content;
      return $this->callApi($url, $params);
    }
    $item = $content;
    if ($item->hasBit(Meta::BIT_TWEETED)) return ;
    if (method_exists($item, 'asTweet')) {
      $content = $item->asTweet();
    }
    else {
      $content = $item->getName().' '.$item->asUrl();
    }
    $content = substr($content, 0, 140);
    $params['status'] = $content;
    $ret = (array) $this->callApi($url, $params);
    if (isset($ret['id'])) {
      $item->setBit(Meta::BIT_TWEETED);
    }
    else {
      S('log')->error($ret, __METHOD__);
    }
  }

}