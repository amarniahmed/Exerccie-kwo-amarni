<?php
#
error_reporting((E_ALL & ~E_NOTICE) | E_STRICT);

setlocale(LC_COLLATE, 'fr_FR');
setlocale(LC_CTYPE, 'fr_FR');

define('CORE', 'core');
define('DEVEL', 'devel');

define('TZ', '+01:00');

define('APP_PATH', 'etc/app');
define('BIN_PATH', 'bin');
define('DOC_PATH', 'doc');
define('ETC_PATH', 'etc');
define('INC_PATH', 'etc/inc');
define('LIB_PATH', 'lib');
define('PUB_PATH', 'pub');
define('TMP_PATH', 'pub/tmp');
define('USR_PATH', 'usr');
define('VAR_PATH', 'var');
define('WEB_PATH', 'web');

define('DOC_URL', '/doc');
define('PUB_URL', '/pub');
define('USR_URL', '/usr');
define('WEB_URL', '/web');
define('PIX_URL', '/web/core/images');

define('DEVELOPER',
       in_array($_SERVER['REMOTE_ADDR'],
                array('192.168.0.102', '92.103.81.138')));

$GLOBALS['objects'] = array();

if (php_sapi_name() != 'cli') {
  define('IS_CLI', false);
/*  $blacklist = array('95.28.54.120');
  $addr = $_SERVER['REMOTE_ADDR'] .
          $_SERVER['HTTP_X_FORWARDED_FOR'] .
          $_SERVER['HTTP_CLIENT_IP'];
  foreach ($blacklist as $address) {
    if (strpos($add, $address) !== false) die(0);
  } */
}
else {
  define('IS_CLI', true);
  $directives = array('safe_mode' => 0,
                      'memory_limit' => '256M',
                      'apc.enabled' => 0,
                      'magic_quotes_runtime' => 0,
                      'date.timezone' => 'Europe/Paris',
                      'include_path' => '.',
                      'default_charset' => 'UTF-8',
                      'mbstring.detect_order' => 'UTF-8',
                      'short_open_tag' => 1);
  foreach ($directives as $key => $value) {
    ini_set($key, $value);
  }
}

if (IS_CLI === false || file_exists(INC_PATH.'/platform.conf.inc')) {
  include INC_PATH.'/platform.conf.inc';
  include INC_PATH.'/dbcache.conf.inc';
}

function u($url, $args=null, $locale=null) {
  $manager = S('app')->getManager();
  $locale = $locale ?: S('res')->getLocale();
  if ($manager && !method_exists($manager, 'url')) {
    if ($url instanceof ActiveRecord) {
      $url = $url->asUrl($locale);
    }
    return H::url($url);
  }
  if ($args) {
    $sep = strpos($url, '?') === false ? '?' : '&';
    $url .= $sep . http_build_query($args);
  }
  $args = array('locale' => $locale,
                'url' => $url);
  return call_user_func(array($manager, 'url'), $args);
}

function a($url) {
  $manager = S('app')->getManager();
  if ($manager && !method_exists($manager, 'asset')) {
    return $url;
  }
  $args = array('url' => $url);
  return call_user_func(array($manager, 'asset'), $args);
}

function h($str) {
  return htmlspecialchars($str, ENT_QUOTES|ENT_SUBSTITUTE, 'UTF-8');
}

function c($str) {
  return SH::capitalize($str);
}

function l($key, $bindings=null, $locale=0) {
  if (S('req')->isBack()) {
    if (empty(P('back.admin.locales'))) {
      return $key;
    }
    return GetText::backDict($key, $bindings, $locale);
  }
  if ($bindings === null && $locale === 0) {
    global $words;
    if (!isset($words[$key])) {
      $words[$key] = GetText::dict($key, $bindings, $locale);
    }
    return $words[$key];
  }
  return GetText::dict($key, $bindings, $locale);
}

function S($obj) {
  return Platform::getSingleton($obj);
}

function T($model) {
  if ($model === 'visit') {
    return Visit::TABLE;
  }
  return Model::parameter($model, 'table');
}

function I($model) {
  return Model::parameter($model, 'id');
}

function P($key) {
  return $GLOBALS['parameters'][$key];
}

function __autoload($class) {
  if (isset($GLOBALS['classes'][$class])) {
    $extension = $GLOBALS['classes'][$class];
  }
  else {
    $extension = CORE;
  }
  require $path = LIB_PATH.'/'.$extension.'/class/'.$class.'.class.inc';
}

Platform::getInstance();

function d($msg, $prefix='') {
  $whitelist = array('192.168.0.102', '92.103.81.138');
  if (IS_CLI === false && !in_array($_SERVER['REMOTE_ADDR'], $whitelist)) {
    return ;
  }
  if (IS_CLI) {
    $info = join(' ', $_SERVER['argv']);
  }
  else {
    $info = $_SERVER['REQUEST_URI'];
  }
  if ($GLOBALS['objects']['log'] === true) {
    S('log')->debug($msg, $prefix);
    return ;
  }
  if (is_int($msg) && $msg === 0) {
    $msg = 'INT(0)';
  }
  elseif (is_null($msg)) {
    $msg = 'NULL';
  }
  elseif (is_bool($msg)) {
    $msg = 'BOOL('.intval($msg).')';
  }
  elseif (is_array($msg)) {
    $msg = "\n".var_export($msg, true);
  }
  elseif (is_object($msg) && method_exists($msg, 'debug')) {
    $msg->debug();
  }
  error_log($info.' : '.$msg);
}
