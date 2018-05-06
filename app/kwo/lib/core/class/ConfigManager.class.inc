<?php

class ConfigManager
{
  private static $trace = 0;
  private static $models = array();
  private static $extensions = array();

  const DBCACHE = 'dbcache.conf.inc';
  const JS_PLATFORM = 'web/core/scripts/platform.js';

  public static function checkSyntax($file) {
    $out = null;
    $ret = null;
    exec(Shell::PHP_BIN.' -l '.$file, $out, $ret);
    if ($ret != 0) {
      array_shift($out);
      S('log')->debug($out, 'file: '.$file);
      throw new Exception(join(', ', $out));
    }
    return true;
  }

  public static function platform() {

    if (!is_writable(ETC_PATH)) {
      throw new Exception(ETC_PATH.' is not writable');
    }

    $dbo = S('dbo');
    $log = S('log');

    $platform = '<?php'."\n\n";

    $tmp_path = ETC_PATH.'/cache.tmp';
    $old_path = INC_PATH.'.'.time();

    $regexp = array();
    self::trace('load and check apps');
    foreach (glob(App::PATH.'/*.conf.inc') as $file) {
      self::checkSyntax($file);
      $parameter = array();
      include $file;
      $regexp[$parameter['app.regexp']] = $file;
    }

    $h = array();
    self::trace('load and check extensions');
    foreach (glob(Extension::PATH.'/*/parameters.conf.inc') as $file) {
      self::checkSyntax($file);
      $parameter = array();
      include $file;
      $name = $parameter['name'];
      self::$extensions[$name] = $parameter;
      $h[$parameter['id']] = $name;
    }
    $platform .= '$extensions = '.var_export($h, true).';'."\n\n";

    $h = array();
    self::trace('load and check models');
    foreach (glob(Extension::PATH.'/*/models/*.conf.inc') as $file) {
      self::checkSyntax($file);
      $parameter = array();
      include $file;
      list (,, $extension) = explode('/', $file);
      $name = $parameter['name'];
      if (!is_string($name)) {
        S('log')->error('invalid parameter, name ['.$file.']', __METHOD__);
      }
      $parameter['extension'] = $extension;
      $parameter['foreign_key'] = $name.'_id';
      $parameter['label'] = $parameter['label'] ?: $name;
      if (!isset($parameter['table'])) {
        $parameter['table'] = self::$extensions[$extension]['table_prefix'] . $name;
//S('log')->debug('>> '.$parameter['table']);
      }
      if (!$dbo->hasTable($parameter['table'])) {
//S('log')->debug($file);
        throw new Exception('table ['.$parameter['table'].'] does not exist');
      }
      self::$models[$name] = $parameter;
      $h[$parameter['id']] = $name;
    }
    ksort($h);
    $platform .= '$models = '.var_export($h, true).';'."\n\n";

    $h = array();
    self::trace('load classes');
    foreach (glob(LIB_PATH.'/*/class/*.class.inc') as $file) {
      if (strpos($file, '/'.CORE.'/') !== false) continue ;
      $name = basename($file, '.class.inc');
      if (!preg_match('/^[[:alpha:]]+$/', $extension)) {
        S('log')->trace('invalid extension name ['.$extension.']');
        continue ;
      }
      list (, $extension) = explode('/', $file);
      $h[$name] = $extension;
    }
    $platform .= '$classes = '.var_export($h, true).';'."\n\n";

    self::trace('relations');
    foreach (self::$models as $name => $params) {
      $extension = $params['extension'];
      if (!is_array($params['has_many'])) continue ;
      foreach ($params['has_many'] as $key => $relation) {
        if (!isset($relation['through'])) continue ;
        if ($relation['through'] === true) {
          $table  = self::$extensions[$extension]['table_prefix'];
          $table .= $name < $key ? $name . '_' . $key : $key . '_' . $name;
          $params['has_many'][$key]['through'] = $table;
          if (!$dbo->hasTable($table)) {
            throw new Exception('table ['.$table.'] does not exist');
          }
        }
        if (!isset($params['has_many'][$key]['foreign_key'])) {
          $params['has_many'][$key]['foreign_key'] = $name.'_id';
        }
      }
      self::$models[$name]['has_many'] = $params['has_many'];
    }

    if (!is_dir($tmp_path)) {
      mkdir($tmp_path);
    }
    else {
      foreach (glob($tmp_path.'/*') as $file) {
        unlink($file);
      }
    }

    self::trace('populate models');
    $model_tables = $table2model = $models = $profiles = array();
    $js = '';
    foreach (self::$models as $name => $params) {
      try {
        $model_tables[] = $params['table'];
        $table2model[$params['table']] = $name;
        if ($params['profile_id'] > 0) {
          $profiles[$params['profile_id']] = $name;
        }
        $js .= 'kwo.platform.models["'.$params['id'].'"] = "'.$name.'";'."\n";
        $models[$name] = self::model($name, $params);
      }
      catch (Exception $e) { $log->error($e); }
    }

    $parameter = array();

    self::trace('load item_tables');
    $parameter['item_tables'] = array(Meta::TABLE, T('translation'));
    foreach ($dbo as $table) {
      $table_name = $table->getName();
      if (substr($table_name, -5) === '_safe') continue ;
      if (in_array($table_name, $model_tables)) continue ;
      if (substr($table_name, 0, 12) == 'core_tracker') continue ;
      if ($table->hasFields('model_id', 'record_id')) {
        $parameter['item_tables'][] = $table_name;
      }
    }

    ksort($profiles);

    $parameter['profiles'] = $profiles;
    $parameter['regexp'] = $regexp;
    $parameter['metas'] = $dbo->getTable(Meta::TABLE)->getFields();

    $platform .= '$parameter = '.var_export($parameter, true).';'."\n";

    $stat = Shell::stat();

    self::trace('chown / chgrp : directory');
    chown($tmp_path, $stat['uid']);
    chgrp($tmp_path, $stat['gid']);

    self::trace('write : platform.conf.inc');
    file_put_contents($tmp_path.'/platform.conf.inc', $platform);
    chown($tmp_path.'/platform.conf.inc', $stat['uid']);
    chgrp($tmp_path.'/platform.conf.inc', $stat['gid']);

    self::trace('write : models');
    foreach ($models as $name => $params) {
      $file = $tmp_path.'/'.$name.'.conf.inc';
      $ret  = '<?php'."\n\n";
      $ret .= '$parameters = '.var_export($params, true).';'."\n";
      file_put_contents($file, $ret);
      chown($file, $stat['uid']);
      chgrp($file, $stat['gid']);
    }

    self::trace('write : '.self::JS_PLATFORM);
    file_put_contents(self::JS_PLATFORM, $js);
    chown(self::JS_PLATFORM, $stat['uid']);
    chgrp(self::JS_PLATFORM, $stat['gid']);

  //    return ; /* for tests */

    self::trace('renaming');
    rename(INC_PATH, $old_path);
    rename($tmp_path, INC_PATH);

    if (file_exists($old_path.'/'.self::DBCACHE)) {
      rename($old_path.'/'.self::DBCACHE,
             INC_PATH.'/'.self::DBCACHE);
    }

    self::trace('cleaning');
    foreach (glob($old_path.'/*.*') as $file) {
      unlink($file);
    }
    rmdir($old_path);

    $locker = Platform::LOCKER;

    self::trace('removing '.$locker);
    if (file_exists($locker)) {
      unlink($locker);
      self::trace('locker removal');
    }

    if (!file_exists(INC_PATH.'/'.self::DBCACHE)) {
      self::trace('dbcachebuild');
      self::DBCacheBuild();
    }

    self::trace('end');

  }

  public static function model($model, $params=null) {
    $file = null;
    if ($params === null) {
      $file = INC_PATH.'/'.$model.'.conf.inc';
      if (!file_exists($file)) {
        S('log')->error($file, __METHOD__);
        throw new ActiveRecordException();
      }
      $parameters = null;
      include $file;
      $params = $parameters;
    }

    if (empty($params)) {
      S('log')->error('no params', __METHOD__);
      throw new ActiveRecordException();
    }

    unset($params['properties']);

    $properties = S('dbo')->asHash('SELECT *'
                                   .' FROM '.Property::TABLE
                                   .' WHERE model_id='.$params['id']
                                   .' ORDER BY name',
                                   null, 'name', 'id');
    if (!empty($properties)) {
      $params['properties'] = $properties;
    }

    if ($params['profile_id'] > 0) {
      if (isset($params['belongs_to'])) {
        $params['belongs_to'] = (array) $params['belongs_to'];
        if (!in_array('user', $params['belongs_to'])) {
          $params['belongs_to'][] = 'user';
        }
      }
      else {
        $params['belongs_to'] = array('user');
      }
    }

    foreach (array('belongs_to', 'has_one', 'has_many') as $relation) {
      if (!isset($params[$relation])) continue ;
      $params[$relation] = (array) $params[$relation];
      $h = array();
      foreach ($params[$relation] as $key => $value) {
        if (!is_array($value)) {
          $key = $value;
          $value = array();
        }
        if (count(self::$models) >= 1 && !isset(self::$models[$key])) {
          S('log')->debug(self::$models, __METHOD__);
          S('log')->trace('['.$model.'] relation error: '.
                          'model ['.$key.'] does not exist', __METHOD__);
          continue ;
        }
        elseif (!Model::exists($key)) {
          S('log')->trace('['.$model.'] relation error: '.
                          'model ['.$key.'] does not exist *', __METHOD__);
          continue ;
        }
        $value['foreign_key'] = $key.'_id';
        if (isset($value['through'])) {
          $value['through_cols'] = S('dbo')->getTable($value['through'])->getFields();
        }
        $h[$key] = $value;
      }
      //      S('log')->debug($h);
      $params[$relation] = $h;
    }
    $params['metas'] = !isset($params['metas']) || $params['metas'] == true ? true : false;

    $table = $params['table'];
    $columns = $fields = array();
    foreach (S('dbo')->getTable($table)->reset()->getColumns() as $column) {
      $field = $column['Field'];
      $type = $column['Type'];
      if ($field === 'id') continue ;
      if ($field === 'flags' || $field === 'locales') {
        $params['casts'][$field] = 'bits';
      }
      if ($type === 'date') {
        $params['casts'][$field] = 'date';
      }
      if ($type === 'datetime' || $type === 'timestamp') {
        $params['casts'][$field] = 'datetime';
      }
      if (strpos($type, 'float') !== false) {
        $params['casts'][$field] = 'float';
      }
      $columns[] = $table.'.'.$field.' AS '.$model.'_'.$field;
      $fields[] = $field;
    }

    if (count($columns) < 1) {
      S('log')->error('no columns for model ['.$model.']', __METHOD__);
      return ;
    }

    $params['fields'] = $fields;
    $params['schema'] = join(', ', $columns);

    if (!empty($file)) {
      $ret  = '<?php'."\n\n";
      $ret .= '$parameters = '.var_export($params, true).';'."\n";
      file_put_contents($file, $ret);
      if (IS_CLI) {
        Shell::chown($file);
      }
    }

    Model::reset();

    return $params;

  }

  public static function DBCacheBuild() {
    try {
      $content  = '<?php'."\n\n";
      $data = Parameter::asCache();
      $content .= '$parameters = '.var_export($data, true).';'."\n\n";
      $data = Redirection::asCache();
      $content .= '$redirections = '.var_export($data, true).';'."\n\n";
      $data = Goal::asCache();
      $content .= '$goals = '.var_export($data, true).';'."\n\n";
      $data = Privilege::asCache();
      $content .= '$middle = '.var_export($data, true).';'."\n\n";
      $data = Locale::asCache();
      $content .= '$locales = '.var_export($data, true).';'."\n\n";
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
      $content = '<?php'."\n\n";
    }
    $file = INC_PATH.'/'.self::DBCACHE;
    file_put_contents($file, $content);
    $stat = Shell::stat();
    chown($file, $stat['uid']);
    chgrp($file, $stat['gid']);
  }

  private static function trace($msg) {
    if (self::$trace < 1) return ;
    S('log')->trace('step : '.$msg);
  }

}


/*    self::trace('relations');
    foreach ($models as $name => $parameters) {
      if (!$parameters['has_many']) continue ;
      foreach ($parameters['has_many'] as $key => $relation) {
        if ($relation['through'] === null) continue ;
        if ($models[$key]['has_many'][$name] != null) continue ;
        $relation['foreign_key'] = $name.'_id';
        $parameters['has_many'][$name] = $relation;
      }
    } */