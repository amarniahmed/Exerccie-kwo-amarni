<?php

class TumblrClient extends OAuthv1Client
{
  protected $api_url = 'http://api.tumblr.com/v2/blog';
  protected $authorize_url = 'http://www.tumblr.com/oauth/authorize';
  protected $oauth_url = 'http://www.tumblr.com';

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
    return new TumblrClient($tokens, $opts);
  }

  /*-------------------- API --------------------*/

  /* http://www.tumblr.com/api_docs#posting */
  public function createPost($blog, $params, $opts=null) {
    $opts = (array) $opts;
    $required = array('title', 'body');
    foreach ($required as $param) {
      if (empty($params[$param])) {
        S('log')->error('param ['.$param.'] required', __METHOD__);
        throw new Exception('invalid params');
      }
    }
    $url = '/'.$blog.'/post';
    if ($params instanceof ActiveRecord) {
      $item = $params;
      $params = $item->asTumblrHash();
    }
    $params['type'] = $opts['type'] ?: 'text';
    $ret = (array) $this->callApi($url, $params);
    if (isset($ret['response']['id'])) {
      S('log')->trace('tumblr post id ('.$ret['response']['id'].')',
                      __METHOD__);
    }
    else {
      S('log')->error($ret, __METHOD__);
    }
  }

}