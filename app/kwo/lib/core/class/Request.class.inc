<?php

class Request
{
  const COOKIE_TRACE_NAME = 'tracecontent';

  private static $parameters = array();
  private static $trace_session = null;

  const FORMAT_JSON    = 1;
  const FORMAT_JSONRPC = 2;
  const FORMAT_REST    = 3;
  const FORMAT_SOAP    = 4;
  const FORMAT_XML     = 5;
  const FORMAT_XMLRPC  = 6;

  const METHOD_GET  = 'GET';
  const METHOD_POST = 'POST';
  const METHOD_HEAD = 'HEAD';

  public static $formats = array(self::FORMAT_JSON => 'json',
                                 self::FORMAT_JSONRPC => 'jsonrpc',
                                 self::FORMAT_REST => 'rest',
                                 self::FORMAT_SOAP => 'soap',
                                 self::FORMAT_XML => 'xml',
                                 self::FORMAT_XMLRPC  => 'xmlrpc');
  const XHR_EXEC   = 1;
  const XHR_UPDATE = 2;

  public function __construct() {}

  public function getContent() { $this->getBody(); }

  public function getBody() {
    return file_get_contents('php://input');
  }

  public function getStreamPath() {
    return 'php://input';
  }

  public function getStream() {
    $stream = @fopen($this->getStreamPath(), 'r');
    if (!$stream) {
      S('log')->error('open stream error', __METHOD__);
    }
    return $stream;
  }

  public function isAccount() { return $this->getScope() === 'account'; }
  public function isApi() { return $this->getScope() === 'api'; }
  public function isBack() { return $this->getScope() === 'back'; }
  public function isCli() { return $this->getScope() === 'cli'; }
  public function isCron() { return $this->isCli() && isset($_ENV['MAILTO']); }
  public function isFront() { return $this->getScope() === 'front'; }
  public function isMiddle() { return $this->getScope() === 'middle'; }

  public function isTraced() {
    if (self::$trace_session === null) {
      if ($this->isCli()) {
        $session = 0;
      }
      else {
        $session = $_COOKIE[self::COOKIE_TRACE_NAME];
      }
      self::$trace_session = (int) $session;
    }
    return self::$trace_session;
  }

  public function isIe($version=null) {
    $regex = $version ? 'MSIE '.strval($version) : 'MSIE';
    return stripos($_SERVER['HTTP_USER_AGENT'], $regex) !== false;
  }

  public function isChrome() {
    return stripos($_SERVER['HTTP_USER_AGENT'], 'chrome') !== false;
  }

  public function isHttp() { return strlen($_SERVER['HTTP_HOST']) > 1; }

  public function isXhr($type=null) {
    $ret = $this->hasHeader('x-prototype-version');
    if ($ret === false) {
      return stripos($_SERVER['HTTP_X_REQUESTED_WITH'], 'XMLHttpRequest') !== false;
    }
    if ($type !== null) {
      if ($type === self::XHR_UPDATE) {
        return $this->getHeader('x-kwo-request') === 'update';
      }
      else {
        return $this->getHeader('x-kwo-request') === 'exec';
      }
    }
    return true;
  }

  public function isPhp() {
    return $this->getHeader('x-kwo-requested-with') === 'php';
  }

  public function isRpc() {
    return $this->getParameter('api_version') >= 1
         ? $this->getParameter('api_version')
         : false;
  }

  public function hasValidCaptcha() {
    try {
      Captcha::check();
      return true;
    }
    catch (Exception $e) {
      $this->track('captcha', Tracker::CATEGORY_ERROR);
      return false;
    }
  }

  public function getChecksum($type='CRC32') {
    if ($this->hasParameter('hash')) {
      return $this->getParameter('hash');
    }
    $attrs = $this->getAttributes();
    foreach (Tracker::$marketing_params as $param) {
      if (!isset($attrs[$param])) continue ;
      unset($attrs[$param]);
    }
    ksort($attrs);
    //    $location = strtr(trim($this->getPath(), '/'), '/.-[]()', '_______');
    //    $location
    //    $location = empty($location) ? 'index' : $location;
    $data  = $this->getScope().$this->getExtension().$this->getAction();
    $data .= AH::encode($attrs);
    if ($type === 'MD5') {
      $h = hash('md5', $data);
    }
    else {
      $h = sprintf('%u', crc32($data));
    }
    $this->setParameter('hash', $h);
    return $h;
  }

  public function getHeader($key) {
    $key = strtolower($key);
    $headers = $this->getParameter('headers');
    if (!is_array($headers)) return false;
    return array_key_exists($key, $headers) ? $headers[$key] : false;
  }

  public function getHeaders() {
    return $this->getParameter('headers');
  }

  public function hasHeader($key) {
    $headers = $this->getParameter('headers');
    return is_array($headers) && array_key_exists(strtolower($key), $headers);
  }

  public function getScope() { return $this->getParameter('scope'); }

  public function getExtension() { return $this->getParameter('extension'); }

  public function getAction() { return $this->getParameter('action'); }

  public function isPreview() {
    return $this->hasAttribute('p') && $this->p === 'y';
  }

  public function setErrorController() {
    return $this->setController('front', CORE, 'server.error');
  }

  public function setController($scope, $extension, $action, $path=null) {
    $this->setParameter('scope', $scope);
    $this->setParameter('extension', $extension);
    $this->setParameter('action', $action);
    $controller = $path ?: LIB_PATH.'/'.$extension.'/'.$scope.'/'.$action.'.inc';
    $this->setParameter('controller', $controller);
    return $controller;
  }

  public function getController() {
    return $this->getParameter('controller');
  }

  public function getAppId() {
    return S('ctx')->getAttribute('_app_id') ?: 1;
  }

  public function getLocale() {
    if ($this->getParameter('locale') >= 1) {
      return $this->getParameter('locale');
    }
    $locales = S('app')->getLocales();
    if (count($locales) === 1 || $this->isCli()) {
      $locale = $locales[0];
    }
    elseif ($this->hasCookie() &&
            $this->getCookie()->hasAttribute('locale')) {
      $locale = $this->getCookie()->getAttribute('locale');
    }
    elseif ($this->hasAttribute('request_locale')) {
      $locale = $this->getAttribute('request_locale');
    }
    else {
      $lang = trim($_SERVER['HTTP_ACCEPT_LANGUAGE']);
      $lang = substr($lang, 0, 2);
      $lang = strtolower($lang);
      $langs = array_flip(I18N::$langs);
      $locale = $langs[$lang];
    }
    $locale = in_array($locale , $locales) ? $locale : $locales[0];
    $this->setParameter('locale', $locale);
    if ($this->hasCookie()) {
      $cookie = $this->getCookie();
      if ($cookie->locale != $locale) {
        $cookie->setAttribute('locale', $locale);
      }
    }
    return $locale;
  }

  public function isSecure() {
    return $_SERVER['HTTP_X_FORWARDED_PORT'] == 'on' ||
           $_SERVER['HTTP_X_FORWARDED_PROTO'] === 'https';
  }

  public function getServer($key) {
    if ($key === 'host') {
      // ServerAlias
      $host = $this->isCli()
            ? $_SERVER['HOSTNAME']
            : $_SERVER['HTTP_HOST'];
      return strtolower($host);
    }
    if ($key === 'port') {
      if (strlen($_SERVER['HTTP_X_FORWARDED_PORT'], '.') >= 1) {
        return $_SERVER['HTTP_X_FORWARDED_PORT'];
      }
      return $_SERVER['SERVER_PORT'];
    }
    if ($key === 'port') {
      if (strlen($_SERVER['HTTP_X_FORWARDED_PORT'], '.') >= 1) {
        return $_SERVER['HTTP_X_FORWARDED_PORT'];
      }
      return $_SERVER['SERVER_PORT'];
    }
    $h = array();
    $h['host'] = $this->getServer('host');
    return $h;
  }

  public function getClient($key=null) {
    if ($key === 'agent') {
      if (!$this->hasParameter('client_agent')) {
        $agent = IS_CLI ? 'shell' : strtolower($_SERVER['HTTP_USER_AGENT']);
        $this->setParameter('client_agent', $agent);
      }
      return $this->getParameter('client_agent');
    }
    if ($key === 'host') {
      if (!$this->hasParameter('client_host')) {
        $this->setParameter('client_host',
                            gethostbyaddr($this->getClient('address')));
      }
      return $this->getParameter('client_host');
    }

    if ($key === 'address') {
      if ($this->hasParameter('client_address')) {
        return $this->getParameter('client_address');
      }
      $this->setParameter('client_address', '?');
      if ($this->isPhp()) {
        $address = $this->getHeader('x-knx-address-from');
      }
      elseif (!empty($_SERVER['HTTP_CLIENT_IP'])) {
        $address = $_SERVER['HTTP_CLIENT_IP'];
      }
      elseif (strpos($_SERVER['HTTP_X_FORWARDED_FOR'], '.') > 1) {
        $this->setParameter('client_is_proxied', true);
        $address = $_SERVER['HTTP_X_FORWARDED_FOR'];
        if (strpos($address, ',') > 1) {
          S('log')->trace(' [XFF='.$_SERVER['HTTP_X_FORWARDED_FOR'].']'.
                          ' [CIP='.$_SERVER['HTTP_CLIENT_IP'].']',
                          __METHOD__);
          $addresses = explode(',', $address);
          //          $address = array_pop($addresses);
          $address = array_shift($addresses);
          $address = trim($address);
        }
        if (IH::isPrivateAddress($address)) {
          $address = '';
        }
      }
      elseif (!empty($_SERVER['HTTP_X_CLUSTER_CLIENT_IP'])) {
        $address = $_SERVER['HTTP_X_CLUSTER_CLIENT_IP'];
        S('log')->trace('cluster ['.$address.']', __METHOD__);
      }
      $address = trim($address);
      if (empty($address)) {
        $address = $_SERVER['REMOTE_ADDR'];
      }
      else {
        $this->setParameter('client_is_proxied', true);
      }
      $this->setParameter('client_address', $address);
      return $this->getParameter('client_address');
    }
    if ($key === 'ip') {
      if (!$this->hasParameter('client_ip')) {
        $this->setParameter('client_ip',
                            IH::ip2num($this->getClient('address')));
      }
      return $this->getParameter('client_ip');
    }
    if ($key === 'referer') {
      if (!$this->hasParameter('referer')) {
        $referer = $this->hasHeader('x-kwo-href')
                 ? $this->getHeader('x-kwo-href')
                 : $_SERVER['HTTP_REFERER'];
        $referer = strtolower($referer);
        $this->setParameter('referer', $referer);
      }
      return $this->getParameter('referer');
    }
    if ($key === 'country') {
      if (!$this->hasParameter('client_country')) {
        $this->setParameter('client_country',
                            IH::ip2country($this->getClient('address')));
      }
      return $this->getParameter('client_country');
    }
    if ($key === 'login') {
      return $_SERVER['PHP_AUTH_USER'];
    }
    if ($key === 'password') {
      return $_SERVER['PHP_AUTH_PW'];
    }
    $h = array();
    if (IS_CLI) return array();
    $h['address'] = $this->getClient('address');
    $h['host']    = $this->getClient('host');
    $h['agent']   = $this->getClient('agent');
    $h['referer'] = $this->getClient('referer');
    if ($this->isAuthenticated()) {
      $user = $this->getUser();
      $h['user'] = $user->getName().' ('.$user->getId().')';
      $h['email'] = $user->email;
    }
    return $h;
  }

  public function getMethod() {
    return strtoupper($_SERVER['REQUEST_METHOD']);
  }

  public function isPost() {
    return $this->getMethod() === self::METHOD_POST;
  }

  public function isGet() {
    return $this->getMethod() === self::METHOD_GET;
  }

  public function isHead() {
    return $this->getMethod() === self::METHOD_HEAD;
  }

  public function getContentType() {
    return $this->getHeader('content-type');
  }

  public function getUrl() {
    // ex. http://monsite.com/ext/mapage?key=value
    $url = $_SERVER['SCRIPT_URI'];
    if (!empty($_SERVER['QUERY_STRING'])) {
      $url .= '?'.$_SERVER['QUERY_STRING'];
    }
    return $url;
  }

  public function getUri() {
    // ex. /ext/mapage?key=value
    return $_SERVER['REQUEST_URI'];
  }

  public function getPath() {
    // ex. /ext/mapage
    return $_SERVER['SCRIPT_URL'];
  }

  public function match($regexp) {
    return (bool) preg_match('#'.$regexp.'#i', $this->getPath());
  }

  public function comesFrom($regexp) {
    return (bool) preg_match('#'.$regexp.'#i', $this->getClient('referer'));
  }

  public function getQueryString($as_hash=false) {
    return $as_hash === true ? $_GET : $_SERVER['QUERY_STRING'];
  }

  public function getSession() {
    if (Platform::hasSingleton('session')) {
      return S('session');
    }
    if ($this->isCli() || $this->isApi()) {
      S('log')->error('no session in scope '.$this->getScope(), __METHOD__);
      throw new Exception('no session in mode '.$this->getScope());
    }
    return Platform::addSingleton('session', new Session());
  }

  public function hasSession($name=Session::COOKIE_NAME) {
    return $this->hasCookie($name);
  }

  /*** AUTH ***/

  public function isAuthenticated() {
    if ($this->isCli()) return false;
    $ctx = S('ctx');
    if (!is_array($ctx->getAttribute('_user'))) {
      AuthManager::getInstance();
    }
    return $ctx->_user['id'] >= 1;
  }

  public function getUserId() {
    S('res')->disableCache();
    $ctx = S('ctx');
    if (!is_array($ctx->getAttribute('_user'))) {
      AuthManager::getInstance();
    }
    if ($ctx->_user['id'] < 1) {
      throw new AuthException('authentication failure');
    }
    return (int) $ctx->_user['id'];
  }

  public function getUser() {
    S('res')->disableCache();
    if ($this->getParameter('user') === null) {
      $user = User::getInstance($this->getUserId());
      if ($user->status < 1) {
        throw new AuthException('authentication failure');
      }
      $this->setParameter('user', $user);
    }
    return $this->getParameter('user');
  }

  public function isModerator() {
    return $this->hasPrivilege(Privilege::MODERATOR);
  }

  public function hasPrivilege($id) {
    S('res')->disableCache();
    if (!$this->isAuthenticated()) return false;
    $session = $this->getSession();
    $user = $session->user;
    return ($user['privileges'] & NH::asBit($id)) > 0;
  }

  public function invalidate() {
    $ctx = S('ctx');
    $ctx->_user['id'] = 0;
    $this->setParameter('user', null);
    if ($this->hasSession()) {
      $this->getSession()->logout();
    }
  }

  /*** /AUTH ***/

  public function isCachable() {
    if ($this->isCli()) return false;
    if (P('cache.scopes')) {
      $scopes = (array) P('cache.scopes');
      if (!in_array($this->getScope(), $scopes)) return false;
    }
    if (P('cache.methods')) {
      $methods = (array) P('cache.methods');
      if (!in_array(strtolower($this->getMethod()), $methods)) return false;
    }
    if (P('cache.content-types')) {
    }
    return true;
  }

  public function isCached() {
    if ($this->isCli()) return false;
    $etag = $this->getHeader('if-none-match');
    if (strpos($etag, '-') < 4) return false;
    list ($expire, $user_id) = explode('-', $etag);
    if (time() <= $expire &&
        intval($user_id) == intval(S('ctx')->_user['id'])) {
      S('res')->sendHttpNotModified($etag);
      return true;
    }
    return false;
  }

  public function getCookie($name=Cookie::NAME, $ttl=null) {
    if (isset(Cookie::$stack[$name])) {
      return Cookie::$stack[$name];
    }
    $cookie = new Cookie($name, $ttl);
    Cookie::$stack[$name] = $cookie;
    return $cookie;
  }

  public function hasCookie($name=Cookie::NAME) {
    if ($this->isCli() || $this->isApi()) return false;
    return array_key_exists($name, $_COOKIE);
  }

  public function removeCookies() {
    foreach ($_COOKIE as $key => $value) {
      $cookie = $this->getCookie($key);
      $cookie->remove();
    }
    return count(Cookie::$stack);
  }

  // on trace l'accès au serveur : incompatible varnish
  public function track($action=true, $category=null) {
    if (P('app.load') > 0 || P('app.tracker') === false) return ;
    S('res')->disableCache();
    list ($action, $category) = Tracker::toEvent($action, $category);
    $tracker = Tracker::getInstance();
    $tracker->track($action, $category);
  }

  public function isBot() {
    if (P('app.tracker') === false) return false;
    Tracker::getInstance();
    return $this->getParameter('is_bot') === true;
  }

  public function isNewVisitor() {
    if (P('app.tracker') === false) return false;
    Tracker::getInstance();
    return (bool) $this->getParameter('is_new_visitor');
  }

  public function isNewVisit() {
    if (P('app.tracker') === false) return false;
    Tracker::getInstance();
    return (bool) $this->getParameter('is_new_visit');
  }

  public function getVisitNumber() {
    if (P('app.tracker') === false) return 0;
    Tracker::getInstance();
    return (int) $this->getParameter('visit_number');
  }

  public function getVisitId() {
    if (P('app.tracker') === false) return 0;
    Tracker::getInstance();
    return (int) $this->getParameter('visit_id');
  }

  public function getVisitorId() {
    if (P('app.tracker') === false) return 0;
    Tracker::getInstance();
    return (int) $this->getParameter('visitor_id');
  }

  public function getVisit() {
    $id = $this->getVisitId();
    if ($id < 1) {
      S('log')->error('invalid visit : '.$id, __METHOD__);
      throw new Exception('invalid visit');
    }
    return Visit::getInstance($id);
  }

  public function trace() {
    //    $str  = $_SERVER['HTTP_HOST'];
    $str = $_SERVER['SCRIPT_URI'];
    if (!empty($_SERVER['QUERY_STRING'])) {
      $str .= '?'.$_SERVER['QUERY_STRING'];
    }
    S('log')->trace($str);
  }

  public function debug($full=false) {
    if ($full === false) {
      S('log')->debug($this->getAttributes(), __METHOD__);
      return ;
    }
    $h = array('PARAMETERS' => $this->getParameters(),
               'ATTRIBUTES' => $this->getAttributes());
    if (!empty($_FILES)) {
      $h['_FILES'] = $_FILES;
    }
    //    $h['_SERVER'] = $_SERVER;
    if ($this->hasCookie()) {
      $h['COOKIE'] = $this->getCookie()->getAttributes();
    }
    $h['CLIENT'] = $this->getClient();
    $h['URL'] = $this->getUrl();
    S('log')->debug($h, __METHOD__);
  }

  public function getBranches() {
    return (array) $this->getAttribute('branches');
  }

  public function getBranch() {
    return (array) $this->getAttribute('branch');
  }

  public function getDialogs() {
    $dialogs = (array) $this->getAttribute('dialogs');
    $dialogs = array_reverse($dialogs, false);
    return $dialogs;
  }

  public function getBranchValue($key, $bubble=false) {
    $branch = (array) $this->getAttribute('branch');
    if ($branch[$key]) {
      return $branch[$key];
    }
    if ($bubble === false) return null;
    $branches = (array) $this->getAttribute('branches');
    if ($branches[$key]) {
      return $branches[$key];
    }
    return null;
  }

  public function hasParameter($key) {
    return array_key_exists($key, self::$parameters);
  }

  public function setParameters($params) {
    foreach ($params as $key => $value) {
      $this->setParameter($key, $value);
    }
  }

  public function setParameter($key, $value) {
    self::$parameters[$key] = $value;
  }

  public function getParameter($key) {
    return self::$parameters[$key];
  }

  public function getParameters() {
    return self::$parameters;
  }

  public function hasAttribute($key, $has_content_flag=false) {
    if (!property_exists($this, $key)) return false;
    $value = $this->getAttribute($key);
    return ($has_content_flag === true && empty($value)) ? false : true;
  }

  public function hasAttributes() {
    if (func_num_args() < 1) return false;
    $attrs = is_array(func_get_arg(0)) ? func_get_arg(0) : func_get_args();
    $has_content_flag = false;
    if (is_bool($attrs[count($attrs) - 1])) {
      $has_content_flag = array_pop($attrs);
    }
    foreach ($attrs as $attr) {
      if (!$this->hasAttribute($attr, $has_content_flag)) return false;
    }
    return true;
  }

  public function getAttribute($key) {
    return $this->{$key};
  }

  public function getAttributes() {
    $attrs = func_get_args();
    $result = array();
    if (empty($attrs)) {
      $result = get_object_vars($this);
      return $result;
    }
    $attrs = is_array($attrs[0]) ? $attrs[0] : $attrs;
    foreach ($attrs as $attr) {
      $result[$attr] = $this->{$attr};
    }
    return $result;
  }

  public function setAttribute($attr, $value) {
    $this->{$attr} = $value;
  }

  public function cleanAttributes() {
    $attrs = get_object_vars($this);
    foreach ($attrs as $attr => $value) {
      if (empty($value)) continue ;
      if (is_array($value)) {
        array_walk_recursive($value, array(self, 'cleanAttribute'));
      }
      else {
        $this->cleanAttribute($value, $attr);
      }
      $this->setAttribute($attr, $value);
    }
  }

  private function cleanAttribute(&$value, $attr) {
    if (!preg_match('/[\w-]*/', $attr)) {
      S('log')->trace('dangerous attribute name ['.$attr.']', __METHOD__);
    }
    $orig = $value;
    if (is_object($value)) {
      S('log')->error('attr ['.$attr.'] should not be an object');
      S('log')->backtrace();
      return ;
    }
    $value = strip_tags($value);
    $value = str_replace(array('<', '>'), array(' ', ' '), $value);
/*    if ($orig != $value) {
      S('log')->trace($orig.' -> '.$value, __METHOD__);
    } */
  }

  public static function format($format) {
    if (!is_numeric($format)) {
      $formats = array_flip(self::$formats);
      $format = $formats[$format];
    }
    if (!Request::$formats[$format]) {
      $format = self::FORMAT_REST;
    }
    return $format;
  }

  public static function getInstance() {
    if (Platform::hasSingleton('req')) {
      return Platform::getSingleton('req');
    }
    $req = new Request();
    Platform::addSingleton('req', $req);
    $router = Router::getInstance();
    $router->dispatch($req);
    $router = null;
  }

}
