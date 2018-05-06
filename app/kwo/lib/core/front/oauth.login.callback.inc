<?php

try {

  $res->useTemplates(null, true);

  $oauth_params = P('oauth.providers');
  $session = $req->getSession();

  if ($req->hasAttribute('denied', true)) {
    // gérer le denied (fermer popup)
  }

  if ($req->state == 'google') {
    $provider = OAuth::PROVIDER_GOOGLE;
    $code = $req->code;
    $params = array('client_id' => $oauth_params['google']['client_id'],
                    'client_secret' => $oauth_params['google']['client_secret'],
                    'redirect_uri' => H::url(OAuth::LOGIN_CALLBACK_URL),
                    'code' => $code);
    $client = GoogleClient::getInstance($params);
    $h = $client->getAccessToken($params);
    $token = $h['access_token'];
    $url = H::url('https://www.googleapis.com/oauth2/v1/userinfo',
                  array('access_token' => $token));
    $usr = json_decode(file_get_contents($url), true);
    $usr['username'] = $usr['name'];
  }
  elseif ($req->state == 'facebook') {
    $provider = OAuth::PROVIDER_FACEBOOK;
    $code = $req->code;
    $params = array('client_id' => $oauth_params['facebook']['client_id'],
                    'client_secret' => $oauth_params['facebook']['client_secret'],
                    'redirect_uri' => H::url(OAuth::LOGIN_CALLBACK_URL),
                    'code' => $code);
    $client = FacebookClient::getInstance($params);
    $h = $client->getAccessToken();
    $token = $h['access_token'];
    $resp = $client->fetch('GET', FacebookClient::GRAPH_URL.'/me', $h);
    $usr = json_decode($resp, true);
    $usr['username'] = $usr['name'];
  }
  elseif ($req->state == 'linkedin') {
    $provider = OAuth::PROVIDER_LINKEDIN;
    $params = array('client_id' => $oauth_params['linkedin']['client_id'],
                    'client_secret' => $oauth_params['linkedin']['client_secret']);
    $client = LinkedInClient::getInstance($params);

    if ($req->hasAttribute('error', true)) {
      S('log')->error(array('code' => $req->error,
                            'message' => $req->error_description));
    }
    else {
      $code = $req->code;
      $params = array('grant_type' => 'authorization_code',
                      'code' => $code,
                      'redirect_uri' => H::url(OAuth::LOGIN_CALLBACK_URL.'?state=linkedin'));
      $h = $client->getAccessToken($params);
      $token = $h['access_token'];
      $params = array('oauth2_access_token' => $h['access_token'],
                      'format' => 'json');
      $usr = $client->getProfileInfos($params);
    }
  }
  elseif ($req->state == 'wordpress') {
    $provider = OAuth::PROVIDER_WORDPRESS;
    $code = $req->code;
    $params = array('client_id' => $oauth_params['wordpress']['client_id'],
                    'client_secret' => $oauth_params['wordpress']['client_secret'],
                    'redirect_uri' => H::url(OAuth::LOGIN_CALLBACK_URL),
                    'code' => $code);
    $client = WordPressClient::getInstance($params);
    $h = $client->getAccessToken();
    $token = $h['access_token'];
    $resp = $client->fetch('GET', WordPressClient::GRAPH_URL.'/me', $h);
    $usr = json_decode($resp, true);
    $usr['username'] = $usr['name'];
  }
  else if ($req->state == 'live') {
    $provider = OAuth::PROVIDER_LIVE;
    $code = $req->code;
    $client_id = $oauth_params['live']['client_id'];
    $client_secret = $oauth_params['live']['client_secret'];
    $url = 'https://login.live.com/oauth20_token.srf';
    $params = array('client_id' => $client_id,
                    'client_secret' => $client_secret,
                    'redirect_uri' => H::url(OAuth::LOGIN_CALLBACK_URL),
                    'code' => $code,
                    'grant_type' => 'authorization_code');
    $response = HttpClient::post($url, $params);
    $h = json_decode($response, true);
    $oauth = $h;
    $access_token = $h['access_token'];
    $url = H::url(LiveClient::USER_INFO_URL, array('access_token' => $access_token));
    $usr = json_decode(@file_get_contents($url), true);
    $usr['username'] = $usr['name'];
    $usr['email'] = $usr['emails']['preferred'];
  }
  else if (strlen($req->oauth_verifier) > 4) {
    $provider = OAuth::PROVIDER_YAHOO;
    $consumer_key = $oauth_params['yahoo']['consumer_key'];
    $consumer_secret = $oauth_params['yahoo']['consumer_secret'];
    $session = $req->getSession();
    $url = H::url('https://api.login.yahoo.com/oauth/v2/get_token',
                  array('oauth_consumer_key' => $consumer_key,
                        'oauth_signature_method' => 'PLAINTEXT',
                        'oauth_version' => '1.0',
                        'oauth_verifier' => $req->oauth_verifier,
                        'oauth_token' => $req->oauth_token,
                        'oauth_timestamp' => time(),
                        'oauth_nonce' => md5($ts),
                        'oauth_signature' => join('&',
                                                  array($consumer_secret,
                                                        $session->getAttribute('oauth_token_secret')))));
    $response = file_get_contents($url);
    $h = null;
    parse_str($response, $h);
    $oauth = $h;
    $access_token = $h['oauth_token'];
    $method = 'GET';
    $url = YahooClient::USER_INFO_URL.'/'.$h['xoauth_yahoo_guid'].'/profile';
    $params = array('format' => 'json');
    $tokens = array('consumer_key' => $consumer_key,
                    'consumer_secret' => $consumer_secret,
                    'oauth_token' => $access_token,
                    'oauth_token_secret' => $h['oauth_token_secret']);
    $headers = array('Authorization' => OAuth::getAuthorizationHeader($method, $url, $params, $tokens, null));
    $opts = array('method' => $method,
                  'headers' => $headers);
    $response = HttpClient::call($url, $params, $opts);
    $usr = json_decode($response, true);
    $usr['email'] = $usr['profile']['emails'][0]['handle'];
    $usr['id'] = $usr['profile']['guid'];
    $usr['username'] = $usr['profile']['nickname'];
    $usr['gender'] = $usr['profile']['gender'] == 'F' ? 'female' : 'male';
  }

  $oauth = array();

  if (is_array($usr) && !empty($usr)) {
    $email = $usr['email'];
    $uid = $usr['id'];
    $id = $dbo->fetchValue('SELECT user_id FROM '.OAuth::TABLE_LOGIN
                           .' WHERE uid=:uid:'
                           .  ' AND provider='.$provider,
                           array(':uid:' => $uid));
    if ($id > 0) {
      $user = User::getInstance((int)$id);
    }
    elseif (Email::isValid($email)) {
      if (empty($usr['username'])) {
        list ($username, ) = explode('@', $email);
        $usr['username'] = $username;
      }
      $id = $dbo->fetchValue('SELECT id FROM '.T('user')
                             .' WHERE email=:email:',
                             array(':email:' => $email));
      if ($id > 0) {
        $user = User::getInstance((int)$id);
      }
      else {
        $attrs = array('email' => $email,
                       'name' => $usr['username'],
                       'locale' => $res->getLocale(),
                       'status' => User::STATUS_CONFIRMED);
        $user = User::getInstance(true);
        $user->commit($attrs);
        $user->setPassword(SH::password(8, $email));
        $user->setVisit($req);
        if ($usr['gender'] == 'female') {
          $user->setFlag(User::FLAG_WOMAN);
        }
        $user->onSignup();
      }
      $row = array('user_id' => $user->getId(),
                   'app_id' => $req->getAppId(),
                   'email' => $user->email,
                   'uid' => $uid,
                   'access_token' => $token,
                   'code' => $code,
                   'provider' => $provider,
                   'oauth' => serialize($oauth),
                   'profile' => serialize($usr));
      $dbo->insert(OAuth::TABLE_LOGIN, $row);
      OAuth::setUserProfile($user);
    }
  }

  if ($user instanceof User) {
    $req->getSession()->auth($user);
    $cookie = $req->getCookie();
    $cookie->setAttributes(array('user_id' => $user->getId(),
                                 'user_secret' => substr($user->password, 2, 8)));
    AuthManager::onUserAuth($user, AuthManager::AUTH_OAUTH);
  }

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }