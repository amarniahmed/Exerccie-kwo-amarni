<?php

class Router
{
  const MAX_URI_PARAMS = 5;

  private $req = null;

  public function __construct() {}

  public function dispatch($req) {

    $this->req = $req;

    try {
      if (!IS_CLI) {
        $headers = apache_request_headers();
        $headers = array_change_key_case($headers, CASE_LOWER);
        $this->req->setParameter('headers', $headers);
        //S('log')->debug($headers);
      }
      $this->detectRoute();
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
      $this->traceError();
      if ($req->isApi()) {
        S('err')->add('internal error', ApiManager::ERROR_INTERNAL_SERVER);
        $req->setController($scope, CORE, 'error');
      }
      elseif (IS_CLI) {
        die('action unknown');
      }
      else {
        $req->setErrorController();
      }
    }

    try {
      $this->loadAttributes();
      $this->loadParameters();
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
    }

    if ($req->isBack()) {
      try {
        $this->handleBack();
      }
      catch (Exception $e) {
        S('log')->error('auth failure : '.$this->req->getUri(),
                        __METHOD__);
        $this->req->setErrorController();
      }
    }

  }

  private function handleBack() {
    $req = $this->req;
    $session = $req->getSession();
    $cookie = $req->getCookie();
    $admin_token = $session->getAttribute('admin_token');
//    S('log')->trace($admin_token);
    if (!empty($admin_token)) {
      list ($admin_id, $secret) = explode('-', $admin_token);
      if ($secret != SH::crypt($admin_id)) {
        $req->setController('front', CORE, 'admin.redirect');
        return ;
      }
      $admin_id = (int) $admin_id;
      $admin = Admin::getInstance($admin_id, array('include' => 'policy'));
      Platform::addSingleton('admin', $admin);
    }
    elseif ($cookie->getAttribute('back.access') == 1) {
      $req->setController('front', CORE, 'admin.redirect');
    }
    else {
      throw new Exception();
    }
  }

  private function detectRoute() {
    $req = $this->req;
    if (IS_CLI) {
      $uri = $_SERVER['argv'][1];
    }
    else {
      $uri = $_SERVER['REQUEST_URI'];
      foreach (Redirection::asHash() as $regexp => $hash) {
        if (preg_match('#'.$regexp.'#u', $uri)) {
          $req->setController('front', CORE, 'redirection');
          S('ctx')->redirection = $hash;
          return ;
        }
      }
      $uri = $_SERVER['SCRIPT_NAME'];
    }
    $uri = str_replace('//', '/', $uri);
    $uri = trim($uri, '/');

    if ($uri === Admin::url()) {
      $req->setController('front', CORE, 'admin.signin');
      return ;
    }

    // TODO : ne pas considérer qu'une locale fait 2 chars , cf. fr_be
    if ($uri[2] === '/' || $uri[1] === '/' || strlen($uri) === 2) {
      $str = substr($uri, 0, 2);
      $str = trim($str, '/');
      if (is_numeric($str)) {
        $req->setParameter('api_version', (int) $str);
        $uri = ltrim(substr($uri, 2), '/');
      }
      elseif (($locale = Locale::toNum($str)) > 0) {
        $req->setParameter('res_locale', $locale);
        $uri = ltrim(substr($uri, strlen($str)), '/');
      }
      /*
      elseif (in_array($str, array('cn', 'de', 'en', 'es', 'fr', 'it',
                                   'ja', 'nl', 'pt', 'ru'))) {
        $req->setParameter('res_locale', I18N::locale2num($str));
        $uri = ltrim(substr($uri, strlen($str)), '/');
      }
      */
    }

    if (substr($uri, 0, 5) === 'push/') {
      $uri = 'campaign.' . substr($uri, 5);
    }

    list ($uri, $params) = explode('/-/', $uri, 2);

    if (strlen($params) >= 3) {
      $pairs = (array) explode('/', $params);
      $pairs = array_slice($pairs, 0, (self::MAX_URI_PARAMS * 2));
      $n = count($pairs) - 1;
      if ($n % 2 === 0) $n++;
      for ($i = 0; $i < $n; $i += 2) {
        $req->setAttribute($pairs[$i], (string) $pairs[$i + 1]);
      }
    }

    $scope = $extension = $action = null;
    $parts = (array) explode('/', $uri, 3);
    array_walk($parts, 'trim');

    if (IS_CLI) {
      $scope = 'cli';
      if (count($parts) == 1) {
        $extension = CORE;
        $action = $parts[0];
      }
      else {
        $extension = $parts[0];
        $action = $parts[1];
      }
    }
    elseif ($req->getParameter('api_version') > 0) {
      $scope = 'api';
      $extension = P('app.extension');
      $action = $parts[0];
      if ($action === 'services.xmlrpc') {
        $req->setParameter('api_format', Request::FORMAT_XMLRPC);
        $method = '';
        list ($api_request_params) = xmlrpc_decode_request($req->getBody(), $method);
        $action = $method;
        $req->setParameter('api_request_method', $action);
        $req->setParameter('api_request_params', $api_request_params);
      }
      elseif ($action === 'services.jsonrpc') {
        $req->setParameter('api_format', Request::FORMAT_JSONRPC);
        $json = json_decode($req->getBody(), true);
        if (($ret = json_last_error()) != JSON_ERROR_NONE) {
          S('log')->error('json decoding error ('.$ret.')', __METHOD__);
        }
        $action = $json['method'];
        $req->setParameter('api_request_method', $action);
        $req->setParameter('api_request_params', $json['params']);
        $req->setParameter('api_request_id', $json['id']);
        $req->setParameter('api_format_version', $json['version']);
      }
      elseif ($action === 'services.xml') {
        $req->setParameter('api_format', Request::FORMAT_XML);
        $xml = $req->getBody();
        if (!empty($xml)) {
          $doc = new DOMDocument();
          $doc->preserveWhiteSpace = false;
          @$doc->loadXML('<?xml version="1.0" encoding="UTF-8" ?>' . "\n" . $xml);
          $doc->encoding = 'UTF-8';
          $action = $doc->documentElement->getAttribute('method');
          $params = XH::asHash($doc);
        }
        else {
          $action = 'null';
          $params = array();
        }
        $req->setParameter('api_request_params', $params);
        $req->setParameter('api_request_method', $action);
      }
      else {
        if ($req->getMethod() == 'POST') {
          $body = trim($req->getBody());
          if ($body[0] === '{') {
            $format = Request::FORMAT_JSON;
          }
          elseif ($body[0] === '<') {
            $format = Request::FORMAT_XML;
          }
          else {
            $content_type = $req->getContentType();
            if (strpos($content_type, 'js') > 0) {
              $format = Request::FORMAT_JSON;
            }
            elseif (strpos($content_type, 'xml') > 0) {
              $format = Request::FORMAT_XML;
            }
            else {
              $params = $_POST;
            }
          }
        }
        else {
          $params = $_GET;
        }
        $req->setParameter('api_format', $format);
        if ($format === Request::FORMAT_XML) {
          if (!empty($body)) {
            $doc = new DOMDocument();
            $doc->preserveWhiteSpace = false;
            @$doc->loadXML('<?xml version="1.0" encoding="UTF-8" ?>' . "\n" . $body);
            $doc->encoding = 'UTF-8';
            $params = XH::asHash($doc);
          }
          else {
            $action = 'null';
            $params = array();
          }
        }
        elseif ($format === Request::FORMAT_JSON) {
          $params = json_decode($body, true);
          if (($ret = json_last_error()) != JSON_ERROR_NONE) {
            $action = 'null';
            S('log')->error('json decoding error ('.$ret.')', __METHOD__);
          }
        }
        $req->setParameter('api_request_params', $params);
        $req->setParameter('api_request_method', $action);
      }
      /*
      elseif ($action === 'services.soap') {
        $req->setParameter('api_format', Request::FORMAT_SOAP);
      }
      */
      if (ApiManager::inWhitelist($action)) {
        $req->setParameter('api_version', 1);
        $extension = CORE;
      }
    }
    elseif (in_array($parts[0], array('account', 'middle'))) {
      $scope = $parts[0];
      $extension = P('app.extension');
      $action = $parts[1] ?: P($scope . '.home');
    }
    elseif ($parts[0] === 'back') {
      $scope = 'back';
      $extension = $parts[1];
      $action = $parts[2];
    }
    else {

      $scope = 'front';
      $extension = P('app.extension');

      $routes = (array) P('front.routes');
      foreach ($routes as $route) {
        $regexp = ltrim($route[0], '/');
        $matches = array();
        if (preg_match('#^'.$regexp.'#u', $uri, $matches)) {
          $action = $route[1];
          foreach ($matches as $key => $value) {
            if (is_int($key)) continue ;
            $req->setAttribute($key, $value);
          }
          if (is_array($route[2])) {
            foreach ($route[2] as $key => $value) {
              if (!is_string($key)) continue ;
              if (strpos($key, '[') >= 1) {
                $pair = explode('[', trim($key, ']'));
                $req->{$pair[0]}[$pair[1]] = $value;
              }
              else {
                $req->setAttribute($key, $value);
              }
            }
          }
          break ;
        }
      }

      if ($action === null) {
        $action = $parts[0] ?: P('front.home');
      }
    }

    $chunk = $extension . $action;
    if ($this->isValid($chunk) === false) {
      S('log')->error($chunk, __METHOD__);
      throw new Exception('url format', Error::CODE_INVALID_URL);
    }

    $req->setController($scope, $extension, $action);

    if ($scope === 'back') {
      $controller = $req->getController();
      $virtuals = array('item', 'developer', 'editor', 'file', 'folder', 'bitmap');
      list ($model, $method) = explode('.', $action, 2);
      if (!file_exists($controller) || $method == 'item') {
        $controller_found = false;
        if (Model::exists($model) || $model == 'visit') {
          $extension = $model == 'visit' ? CORE : Model::extension($model);
          $controller = LIB_PATH.'/'.$extension.'/back/'.$model.'.'.$method.'.inc';
          if (file_exists($controller)) {
            $req->setController($scope, $extension, $model.'.'.$method,
                                $controller);
            $controller_found = true;
          }
        }
        if ($controller_found === false) {
          $req->setController($scope, $extension, $action,
                              LIB_PATH.'/'.CORE.'/back/item/'.$method.'.inc');
        }
      }
      if (!empty($method) && $model != 'item' && (Model::exists($model) || $model == 'visit')) {
        define('MODEL', strtolower($model));
        S('ctx')->setAttribute('_is_model', Model::exists(MODEL));
      }
    }
    elseif (!IS_CLI) {
      S('app')->hook('onRequestDispatch', array('req' => $req));
    }

    $controller = $req->getController();

    if (!Extension::exists($req->getExtension())) {
      throw new Exception('invalid extension ['.$req->getExtension().']',
                          Error::CODE_INVALID_EXTENSION);
    }

    if (!file_exists($controller)) {

//      if ($scope === 'front' && in_array($action, FrontManager::$core_whitelist)) {
      if (FrontManager::inWhitelist($action)) {
        $path = LIB_PATH.'/'.CORE.'/front/'.$action.'.inc';
        $controller = $req->setController($scope, $extension, $action, $path);
        if ($controller && file_exists($controller)) return ;
      }
      elseif ($extension === DEVEL) {
        $controller = null;
        if ($scope === 'account') {
          $path = LIB_PATH.'/'.CORE.'/account/'.$action.'.inc';
          $controller = $req->setController($scope, $extension, $action, $path);
        }
        elseif ($scope === 'front') {
          foreach (array('cms', 'shop', 'social') as $dir) {
            $path = LIB_PATH.'/'.CORE.'/front/'.$dir.'/'.$action.'.inc';
            if (file_exists($path)) {
              $controller = $req->setController($scope, $extension, $action, $path);
              break ;
            }
          }
        }
        if ($controller && file_exists($controller)) return ;
      }

      if ($scope === 'api') {
        $req->setController($scope, CORE, 'error');
      }
      else {
        throw new Exception('invalid controller ['.$controller.']',
                            Error::CODE_INVALID_CONTROLLER);
      }
    }

  }

  private function loadAttributes() {
    $req = $this->req;
    if ($req->isCli()) {
      foreach ($_SERVER['argv'] as $key => $value) {
	if ($key <= 1) continue ;
	$pair = explode('=', $value);
	$req->setAttribute($pair[0], trim($pair[1]));
      }
      return ;
    }
    if ($req->isApi() && $req->hasParameter('api_request_params')) {
      $params = $req->getParameter('api_request_params');
      $params = is_array($params) ? $params : array();
      foreach ($params as $key => $value) {
        $req->setAttribute($key, $value);
      }
      return ;
    }
    $attrs = array();
    $attrs += $_POST + $_GET;
    $encoding = null;
    if ($req->isXhr()) {
      $encoding = 'utf8';
    }
    elseif (array_key_exists('kie', $attrs)) {
      $encoding = $attrs['kie'];
    }
    array_walk_recursive($attrs, array($this, 'filterParams'), $encoding);
    foreach ($attrs as $key => $value) {
      if ($key === '__token') continue ;
      $req->setAttribute($key, $value);
    }
  }

  private function filterParams(&$value, $key, $encoding) {
    $value = trim($value);
  }

  private function loadParameters() {
    $req = $this->req;
    $locales = S('app')->getLocales();
    if (count($locales) === 1) {
      $res_locale = $locales[0];
    }
    elseif ($req->hasParameter('res_locale')) {
      $res_locale = $req->getParameter('res_locale');
    }
    elseif ($req->hasHeader('x-knx-res-locale')) {
      $res_locale = $req->getHeader('x-knx-res-locale');
    }
    elseif ($req->hasAttribute('res_locale')) {
      $res_locale = $req->getAttribute('res_locale');
    }
    $req->setParameter('res_locale', $res_locale);
  }

  private function traceError() {
    if (P('app.load') > 0) return ;
    if (IS_CLI) {
      $url = join(' ', $_SERVER['argv']);
    }
    else {
      $url  = '[' . DH::now() . ']' . "\t";
      $url .= $_SERVER['HTTP_X_FORWARDED_FOR'] ?: $_SERVER['REMOTE_ADDR'] . "\t";
      $url .= $_SERVER['SCRIPT_URI'];
      if (!empty($_SERVER['QUERY_STRING'])) {
        $url .= '?' . $_SERVER['QUERY_STRING'];
      }
      if (false) {
        $url .= "\n" . ' * ' . $_SERVER['HTTP_USER_AGENT'];
        $url .= "\n" . ' * ' . $_SERVER['HTTP_REFERER'];
      }
    }
    $fp = fopen(Logger::URL_FILE, 'a');
    fwrite($fp, $url . "\n");
    fclose($fp);
  }

  public function isValid($path) {
    return (empty($path) || (strlen($path) < 64 && preg_match('/^[[:alnum:]_.-]+$/', $path)));
  }

  public static function getInstance() {
    $class = __CLASS__;
    return new $class;
  }

}
