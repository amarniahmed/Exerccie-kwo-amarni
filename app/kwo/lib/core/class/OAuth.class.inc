<?php

class OAuth {

  const PROVIDER_FACEBOOK = 1;
  const PROVIDER_GOOGLE = 2;
  const PROVIDER_YAHOO = 3;
  const PROVIDER_TWITTER = 4;
  const PROVIDER_LIVE = 5;
  const PROVIDER_TUMBLR = 6;
  const PROVIDER_AMAZON = 7;
  const PROVIDER_WORDPRESS = 8;
  const PROVIDER_LINKEDIN = 9;

  const TABLE_LOGIN = 'core_oauth_login';
  const TABLE_TOKEN = 'core_oauth_token';

  const LOGIN_URL = '/oauth.login';
  const LOGIN_CALLBACK_URL = '/oauth.login.callback';

  public static $providers = array(self::PROVIDER_FACEBOOK => 'facebook',
                                   self::PROVIDER_GOOGLE => 'google',
                                   self::PROVIDER_YAHOO => 'yahoo',
                                   self::PROVIDER_TWITTER => 'twitter',
                                   self::PROVIDER_LIVE => 'live',
                                   self::PROVIDER_TUMBLR => 'tumblr',
                                   self::PROVIDER_AMAZON => 'amazon',
                                   self::PROVIDER_WORDPRESS => 'wordpress',
                                   self::PROVIDER_LINKEDIN => 'linkedin');

  public static function getClient($provider, $tokens, $opts=null) {
    $provider = self::provider2id($provider);
    $client = null;
    switch ($provider) {
      case self::PROVIDER_TUMBLR:
        $client = TumblrClient::getInstance($tokens, $opts); break;
      case self::PROVIDER_TUMBLR:
        $client = TwitterClient::getInstance($tokens, $opts); break;
      case self::PROVIDER_GOOGLE:
        $client = GoogleClient::getInstance($tokens, $opts); break;
      case self::PROVIDER_LINKEDIN:
        $client = LinkedInClient::getInstance($tokens, $opts); break;
    }
    return $client;
  }

  public static function login($provider=null) {
    if (empty($provider)) {
      $provider = self::PROVIDER_FACEBOOK;
    }
    $provider = self::provider2id($provider);
    if ($provider == self::PROVIDER_GOOGLE) {
      return GoogleClient::login();
    }
    elseif ($provider == self::PROVIDER_FACEBOOK) {
      return FacebookClient::login();
    }
    elseif ($provider == self::PROVIDER_WORDPRESS) {
      return FacebookClient::login();
    }
    elseif ($provider == self::PROVIDER_LINKEDIN) {
      return LinkedInClient::login();
    }
    else {
      S('log')->error('invalid provider', __METHOD__);
      return S('res')->sendRedirect('/');
    }
  }

  public static function provider2id($provider) {
    if (is_numeric($provider)) return (int) $provider;
    $providers = array_flip(self::$providers);
    return (int) $providers[$provider];
  }

  public static function getSignature($method, $url, $params, $tokens, $oauth) {
    $signature = hash_hmac('SHA1',
                           self::getBaseString($method, $url, $params, $oauth),
                           self::getKey($tokens),
                           true);
    $signature = base64_encode($signature);
    return $signature;
  }

  private static function getBaseString($method, $url, $params, $oauth) {
    $attrs = array_merge($params, $oauth);
    $attrs = self::build_http_query($attrs);
    $parts = array(strtoupper($method), $url, $attrs);
    $base_string = self::encode($parts);
    return $base_string;
  }

  private static function getKey($tokens) {
    $parts = array($tokens['consumer_secret'],
                   $tokens['oauth_token_secret'] ?: '');
    $key = self::encode($parts);
    return $key;
  }

  private static function encode($input) {
    foreach ($input as &$value) {
      $value = rawurlencode($value);
    }
    $str = implode('&', $input);
    return $str;
  }

  public static function build_http_query($params) {
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

  public static function setUserProfile($user, $model=null) {
    if ($model === null) {
      $model = P('profile.user') ?: 'person';
    }
    if ($user->hasProfile($model)) return;
    $rs = S('dbo')->query('SELECT * FROM '.self::TABLE_LOGIN
                          .' WHERE user_id='.$user->getId());
    if ($rs->foundRows() < 1) return;
    $rows = $rs->asArray();
    $row = $rows[0];

    $provider = (int) $row['provider'];
    $datas = unserialize($row['profile']);
    $attrs = array('user_id' => $user->getId());

    if ($provider == self::PROVIDER_FACEBOOK) {
      $attrs['first_name'] = $datas['first_name'];
      $attrs['last_name'] = $datas['last_name'];
    }
    elseif ($provider == self::PROVIDER_GOOGLE) {
      $attrs['first_name'] = $datas['given_name'];
      $attrs['last_name'] = $datas['family_name'];
    }
    elseif ($provider == self::PROVIDER_LINKEDIN) {
      $attrs['first_name'] = $datas['firstName'];
      $attrs['last_name'] = $datas['lastName'];
      if ($datas['dateOfBirth']) {
        $bd = $datas['dateOfBirth'];
        $birth_date = $bd['year'].'-'.$bd['month'].'-'.$bd['day'];
        $user->setInformation('birth_date', $birth_date);
      }
      if (is_array($datas['positions']['values'][0])) {
        $position = $datas['positions']['values'][0];
        $attrs['organization'] = $position['company']['name'];
        $attrs['occupation'] = $position['title'];
      }
    }
    $profile = Item::getInstance($model, true);
    $profile->commit($attrs);
    $profile->setUser($user->getId());
    S('app')->hook('onOAuthUserProfileSet',
                   array('provider' => $provider,
                         'user' => $user,
                         'profile' => $profile,
                         'datas' => $datas));
  }

}