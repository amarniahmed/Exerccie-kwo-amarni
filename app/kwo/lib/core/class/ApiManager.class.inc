<?php

class ApiManager
{

  /*
missing
    This means a resource does not exist.
missing_field
    This means a required field on a resource has not been set.
invalid
    This means the formatting of a field is invalid. The documentation for that resource should be able to give you more specific information.
already_exists
    This means another resource has the same value as this field. This can happen in resources that must have some unique key (such as Label names).
   */

  /* https://dev.twitter.com/docs/error-codes-responses */

/*http://www.rooftopsolutions.nl/blog/new-http-status-codes */

/* X-Warning*/

  const ERROR_BAD_REQUEST = 400;
  const ERROR_UNAUTHORIZED = 401;
  const ERROR_FORBIDDEN = 403;
  const ERROR_NOT_FOUND = 404;
  /* https://dev.twitter.com/docs/error-codes-responses */
  /* https://dev.twitter.com/docs/rate-limiting/faq */
  const ERROR_RATE_LIMIT = 429;
  const ERROR_INTERNAL_SERVER = 500;
  const ERROR_NOT_AVAILABLE = 503;

  private static $whitelist = array('access.token.request', 'error', 'ping',
                                    'pingback.ping', 'rate.status');

  public static function inWhitelist($action) {
    return in_array($action, self::$whitelist);
  }

  public function __construct() {}

  private function isValidCall() {
    if (!P('api.authentication')) return true;
    $req = S('req');
    $action = $req->getAction();
    if (in_array($action, array('access.token.request', 'error'))) {
      return true;
    }
    /***
1/ black list
2/ rate limit
3/ access token ttl ?
     ***/

    if ($req->hasHeader('x-access-token')) {
      $token = $req->getHeader('x-access-token');
    }
    elseif ($req->hasAttribute('access_token')) {
      $token = $req->getAttribute('access_token');
    }
    if (strlen($token) < 5) {
      throw new Exception('unknown credential', ApiManager::ERROR_UNAUTHORIZED);
    }
    list ($user_id, $seal) = explode('-', $token);
    $user_id = (int) $user_id;
    if (!User::exists($user_id)) {
      throw new Exception('invalid credential', ApiManager::ERROR_UNAUTHORIZED);
    }
    $user = User::getInstance($user_id);
    if (!$user->isOnline()) {
      throw new Exception('invalid user', ApiManager::ERROR_UNAUTHORIZED);
    }
    if ($token != self::getAccessToken($user)) {
      throw new Exception('invalid seal', ApiManager::ERROR_UNAUTHORIZED);
    }
    S('ctx')->setAttribute('user', $user);
    return true;
  }

  public static function accept() {
    try {
      $am = new ApiManager();
      return $am->isValidCall();
    }
    catch (Exception $e) {
      S('err')->add($e);
      return false;
    }
  }

  public static function getAccessToken($user) {
    $seal = substr(hash('md5', $user->id . $user->password), 0, 8);
    return $user->id . '-' . $seal;
  }

}