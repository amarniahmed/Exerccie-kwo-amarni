<?php

class App
{
  const PATH = 'etc/app';
  const COLUMN = 'app';

  private $attributes = array();
  public $ts = null;

  public function __construct($parameter) {
    if (!is_array($parameter)) {
      $name = $parameter;
      $parameter = array();
      include self::PATH.'/'.$name.'.conf.inc';
    }
    foreach (array('app.locales', 'back.locales') as $field) {
      if (!$parameter[$field]) continue ;
      $parameter[$field] = (array) $parameter[$field];
      $locales = array();
      foreach ($parameter[$field] as $locale) {
        $locales[] = is_numeric($locale) ? (int) $locale : Locale::toNum($locale);
      }
      $parameter[$field] = $locales;
    }
    $this->attributes = $parameter;
  }

  public function hasManager() {
    $manager = $this->getManager();
    return !empty($manager);
  }

  public function getManager() {
    return $this->getAttribute('app.manager');
  }

  private static $methods = null;

  public function hook($method, $args=null) {
    if (!$this->hasManager()) return false;
    $scope = S('req')->getScope();
    $do_catch = false;
    if (in_array($method, array('onAppStart', 'onAppStop',
                                'onActionStart', 'onActionStop', 'onActionFail'))) {
      if ($scope === 'api' || $scope === 'cli') return ;
      $do_catch = true;
    }
    if (S('req')->getAction() == 'server.error') $do_catch = true;

    $manager = $this->getManager();

    if (self::$methods === null) {
      self::$methods = (array) get_class_methods($manager);
    }

    if (!in_array($method, self::$methods)) {
      return false;
    }

    if ($do_catch === true) {
      try {
        return call_user_func(array($manager, $method), $args);
      }
      catch (Exception $e) {
        S('log')->error($e, __METHOD__);
      }
    }
    else {
      return call_user_func(array($manager, $method), $args);
    }
  }

  public function getId() {
    return (int) $this->getAttribute('app.id');
  }

  public function getName() {
    return $this->getAttribute('app.name');
  }

  public function getLocale() {
    $locales = $this->getAttribute('app.locales');
    $locale = reset($locales);
    return $locale ?: Locale::FR;
  }

  public function getBackLocales($as_code=false) {
    $locales = $this->getAttribute('back.locales');
    if (empty($locales)) {
      $locales = $this->getLocales();
    }
    return $locales;
  }

  public function getLocales($as_code=false) {
    $h = (array) $this->getAttribute('app.locales');
    return $h;
  }

  public function hasAttribute($key) {
    return array_key_exists($key, $this->attributes);
  }

  public function setAttribute($key, $value) {
    $this->attributes[$key] = $value;
  }

  public function getAttributes() {
    return $this->attributes;
  }

  public function getAttribute($key) {
    return $this->attributes[$key];
  }

  public function debug() {
    S('log')->debug($this->attributes);
  }

  public static function all() {
    $apps = array();
    foreach (glob(self::PATH.'/*.inc') as $file) {
      $parameter = array();
      include $file;
      $apps[$parameter['app.name']] = $parameter;
    }
    return $apps;
  }

  public static function name($app_id) {
    foreach (self::all() as $h) {
      if ($app_id == $h['app.id']) return $h['app.name'];
    }
    return 'app_'.$app_id;
  }

  public static function asAttributes($name) {
    $parameter = array();
    $file = self::PATH.'/'.$name.'.conf.inc';
    if (!file_exists($file)) {
      throw new Exception('invalid app name ['.$name.']', __METHOD__);
    }
    include $file;
    return $parameter;
  }

  public static function asSet() {
    $set = array();
    foreach (glob(self::PATH.'/*.inc') as $file) {
      $parameter = array();
      include $file;
      $set[] = $parameter['app.name'];
    }
    return $set;
  }

  public static function extension2app() {
    $h = array();
    foreach (glob(self::PATH.'/*.inc') as $file) {
      $parameter = array();
      include $file;
      $h[$parameter['app.extension']] = array('name' => $parameter['app.name'],
                                              'file' => $file);
    }
    return $h;
  }

  public static function asHash($name) {
    $parameter = array();
    $file = self::PATH.'/'.$name.'.conf.inc';
    if (!file_exists($file)) {
      throw new Exception('invalid app name ['.$name.']', __METHOD__);
    }
    include $file;
    return $parameter;
  }

  public static function names($only_values=false) {
    $apps = array();
    if ($only_values === false) {
      $apps[''] = '--';
    }
    foreach (glob(self::PATH.'/*.conf.inc') as $file) {
      $parameter = array();
      include $file;
      $apps[$parameter['app.name']] = $parameter['app.name'];
    }
    asort($apps);
    return $apps;
  }

  public static function withId() {
    $out = array();
    foreach (self::all() as $h) {
      if ($h['app.id'] < 1) continue ;
      if ($out[$h['app.id']]) continue ;
      $out[$h['app.id']] = $h['app.name'];
    }
    return $out;
  }

  public static function count() {
    return (int) count(P('regexp'));
  }

  public static function getInstance($name=null) {

    if (!is_null($name)) {
      return new App($name);
    }

    foreach (array('extensions', 'models', 'parameters', 'parameter') as $key) {
      $GLOBALS[$key] = (array) $GLOBALS[$key];
    }

    $regexp = $GLOBALS['parameter']['regexp'];
    $parameter = array();
    $app_path = null;
    if (IS_CLI) {
      if (!file_exists(basename($_SERVER['argv'][0]))) {
        throw new Exception('Please changedir in the DocumentRoot directory.');
      }
      if (count($regexp) >= 1 &&
          strlen($_ENV['KWO_APP']) >= 2 &&
          file_exists(self::PATH.'/'.$_ENV['KWO_APP'].'.conf.inc')) {
        $app_path = self::PATH.'/'.$_ENV['KWO_APP'].'.conf.inc';
      }
      else {
        $script = $_SERVER['argv'][1];
        if (strpos($script, '/') > 2) {
          list ($extension, $action) = explode('/', $script);
          $e2a = App::extension2app();
          if ($e2a[$extension]) {
            $app_path = $e2a[$extension]['file'];
          }
          else {
            $files = glob(self::PATH.'/*.conf.inc');
            $app_path = $files[0];
          }
        }
        else {
          $files = glob(self::PATH.'/*.conf.inc');
          $app_path = $files[0];
        }
      }
    }
    else {
      $hostname = $_SERVER['HTTP_HOST'];
      if (count($regexp) === 1 || strlen($hostname) < 1) {
        $app_path = current($regexp);
      }
      else {
        $flag = false;
        foreach ($regexp as $reg => $path) {
          if (preg_match($reg, $hostname) > 0) {
            $flag = true;
            $app_path = $path;
            break ;
          }
        }
        if ($flag === false) {
          $app_path = $path;
        }
      }
    }

    include INC_PATH . '/' . 'platform.conf.inc';
    include $app_path;
    if (empty($parameter['app.seed'])) {
      $parameter['app.seed'] = 'XXXXXXXX';
    }

    $app = new App($parameter);

    $GLOBALS['parameter'] += (array) $app->getAttributes();

    foreach ($GLOBALS['parameter'] as $key => $value) {
      if (isset($GLOBALS['parameters'][$key])) continue ;
      $GLOBALS['parameters'][$key] = $value;
    }

    unset($GLOBALS['parameter']);

    S('ctx')->setAttribute('_app', (string) $app->getName());
    S('ctx')->setAttribute('_app_id', $app->getId());

    $app->ts = time();

    Platform::addSingleton('app', $app);

  }

}
