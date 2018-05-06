<?php

class Extension {

  const PATH = 'etc/ext';

  public $account = null;
  public $back = null;
  public $has_resource = null;
  public $id = null;
  public $label = null;
  public $models = null;
  public $name = null;
  public $table_prefix = null;

  const THRESHOLD = 10000;

  public static $names = null;

  private static $parameters = array();

  public function __construct($name) {
    $this->name = trim($name);
    if (!self::exists($this->name)) {
      S('log')->error('unknown extension ['.$this->name.']', __METHOD__);
      throw new Exception('unknown extension');
    }
    $parameters = self::parameters($this->name);
    if (strlen($this->name) <= 2 || empty($parameters)) {
      S('log')->error('invalid extension ['.$this->name.']', __METHOD__);
      throw new Exception('invalid extension');
    }
    foreach ($parameters as $key => $value) {
      $this->{$key} = $value;
    }
  }

  public function getModels() {
    if (is_array($this->models)) return $this->models;
    $this->models = array();
    foreach (Model::all() as $model) {
      $parameters = Model::parameters($model);
      if ($parameters['extension'] == $this->name) {
        $this->models[$model] = $parameters;
      }
    }
    return $this->models;
  }

  public function getTables() {
    $tables = array();
    if (strlen($this->table_prefix) < 2) {
      return $tables;
    }
    $expr = $this->table_prefix . '*';
    foreach (S('dbo')->getTables($expr) as $name => $table) {
      $tables[$name]++;
    }
    foreach ($this->getModels() as $key => $value) {
      $tables[$value['table']]++;
    }
    return array_keys($tables);
  }

  public function getId() {
    return $this->id;
  }

  public function archive() {
    $tables = $this->getTables();
    if (count($tables) >= 1) {
      $dir = LIB_PATH.'/'.$this->name.'/sql';
      if (!is_dir($dir)) {
        mkdir($dir);
      }
      $cmd = 'mysqldump '.P('app.dsn.name').' '.join(' ', $tables).' > '.$dir.'/tables.sql';
      S('log')->trace($cmd);
      exec($cmd);
    }
    $parts = array('zip -r '.$this->name.'.kwa',
                   LIB_PATH.'/'.$this->name,
                   WEB_PATH.'/'.$this->name);
    if (file_exists(self::PATH.'/'.$this->name)) {
      $parts[] = self::PATH.'/'.$this->name;
    }
    $cmd = join(' ', $parts);
    exec($cmd);
  }

  protected function cleanItemTables($mode) {
    if ($this->name === CORE) return ;
    $dbo = S('dbo');
    $models = array();
    $protected = array('letter', 'locale', 'notice', 'privilege', 'snippet', 'string');
    foreach ($this->getModels() as $model) {
      if ($mode === 'reset' || in_array($model['name'], $protected)) continue ;
      $models[] = $model['id'];
    }
    if (count($models) < 1) return ;
    Item::clean($models);
  }

  public function uninstall() {
    if (in_array($this->name, array(CORE))) {
      return ;
    }

    $dbo = S('dbo');

    foreach ($this->getTables() as $table) {
      try {
        $dbo->getTable($table)->drop();
      }
      catch (Exception $e) { S('log')->error($e); }
    }

    $set = array();
    foreach ($this->getModels() as $model) {
      $set[] = $model['id'];
    }
    
    if (count($set) > 0) {
      $dbo->exec('DELETE FROM '.T('property')
                 .    ' WHERE model_id IN ('.join(',', $set).')');
    }

    $this->cleanItemTables('uninstall');
    
    Shell::removeDir(self::PATH.'/'.$this->name);
    Shell::removeDir(LIB_PATH.'/'.$this->name);
    Shell::removeDir(WEB_PATH.'/'.$this->name);
    if (is_dir(PUB_PATH.'/'.$this->name)) {
      Shell::removeDir(PUB_PATH.'/'.$this->name);
    }
    if (is_dir(VAR_PATH.'/'.$this->name)) {
      Shell::removeDir(VAR_PATH.'/'.$this->name);
    }
    if (is_dir(DOC_PATH.'/'.$this->name)) {
      Shell::removeDir(DOC_PATH.'/'.$this->name);
    }
    if (is_file(APP_PATH.'/'.$this->name.'.conf.inc')) {
      unlink(APP_PATH.'/'.$this->name.'.conf.inc');
    }
  }

  public static function exists($name) {
    if ($name === CORE) return true;
    if (is_numeric($name)) {
      return array_key_exists(intval($name), $GLOBALS['extensions']);
    }
    return in_array($name, $GLOBALS['extensions']);
  }

  public static function parameter($name, $key) {
    $parameters = self::parameters($name);
    return $parameters[$key];
  }

  public static function asAttributes($name) {
    return self::parameters($name);
  }

  public static function parameters($name) {
    if (!isset(self::$parameters[$name])) {
      include self::PATH.'/'.$name.'/parameters.conf.inc';
      self::$parameters[$name] = $parameter;
    }
    return self::$parameters[$name];
  }

  public static function asHash($internal_only=false) {
    $h = array();
    foreach (self::all() as $name => $extension) {
      if ($extension['id'] >= Extension::THRESHOLD && $internal_only == true) continue ;
      $h[$extension['id']] = $extension['name'];
    }
    return $h;
  }

  public static function name($name) {
    return is_numeric($name) ? $GLOBALS['extensions'][intval($name)] : $name;
  }

  public static function id($name) {
    return is_numeric($name) ? $name : self::parameter($name, 'id');
  }

  public static function all() {
    $extensions = array();
    foreach (glob(self::PATH.'/*/parameters.conf.inc') as $file) {
      $parameter = array();
      list (,, $name) = explode('/', $file);
      include $file;
      $extensions[$parameter['name']] = $parameter;
    }
    return $extensions;
  }

  public static function names() {
    return $GLOBALS['extensions'];
  }

  public static function getInstance($name) {
    return new Extension($name);
  }

}