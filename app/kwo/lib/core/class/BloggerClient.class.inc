<?php

// https://developers.google.com/blogger/docs/3.0/using#AboutAuthorization
// https://developers.google.com/accounts/docs/OAuth2WebServer

class BloggerClient
{
  protected $api_url = 'https://www.googleapis.com/blogger/v3/';

  protected $auth_url = 'https://accounts.google.com/o/oauth2/auth';

  public function __construct($opts) {
    $this->oauth = array_merge(array('version' => '1.0',
                                     'signature_method' => 'HMAC-SHA1'),
                               $opts);
  }

  /*** OAuth ***/

  

  /*** /OAuth ***/

  public static function getInstance($opts=null) {
    return new BloggerClient($opts);
  }
}