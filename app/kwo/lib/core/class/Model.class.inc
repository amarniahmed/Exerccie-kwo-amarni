<?php

class Model
{
  private static $ids = null;
  private static $properties = array();
  private static $parameters = array();
//  private static $_properties = array();

  public static function all() {
    asort($GLOBALS['models']);
    return $GLOBALS['models'];
  }

  public static function id($model) {
    if (is_numeric($model)) return intval($model);
    if (self::$ids === null) {
      self::$ids = array_flip($GLOBALS['models']);
    }
    return (int) self::$ids[$model];
  }

  public static function isBitmaskField($model, $field) {
    $model = self::name($model);
    if (!self::hasField($model, $field)) return false;
    $casts = (array) self::parameter($model, 'casts');
    return $casts[$field] === 'bits';
  }

  public static function hasOwner($model) {
    return self::hasFields($model, array('model_id', 'record_id'));
  }

  public static function hasMetas($model) {
    return self::parameter($model, 'metas') === true;
  }

  public static function hasField($model, $key) {
    if (empty($key)) return false;
    $fields = self::parameter($model, 'fields'); 
    return in_array($key, $fields);
  }

  public static function hasFields($model, $keys) {
    $fields = self::parameter($model, 'fields');
    foreach ($keys as $key) {
      if (!in_array($key, $fields)) return false;
    }
    return true;
  }

  public static function isLocalized($model) {
    return self::hasField($model, 'locales');
  }

  public static function hasAnyField($model, $keys) {
    $fields = self::parameter($model, 'fields');
    foreach ($keys as $key) {
      if (in_array($key, $fields)) return true;
    }
    return false;
  }

  public static function name($model) { 
    if ($model instanceof ActiveRecord) return $model->getModel();
    if (empty($model) || (is_string($model) && $model == 'MODEL')) {
      S('log')->debug($model, __METHOD__);
      // S('log')->backtrace();
    }
    $ret = is_numeric($model) ? $GLOBALS['models'][intval($model)] : strtolower($model);
    if (empty($ret)) {
      S('log')->debug($model.' failure ['.$model.']', __METHOD__);
      //      S('log')->backtrace();
    }
    return $ret;
  }

  public static function label($model) {
    return Model::parameter($model, 'label');
  }

  public static function labels() {
    $labels = array();
    foreach ($GLOBALS['models'] as $id => $name) {
      $labels[$id] = Model::label($id);
    }
    asort($labels);
    return $labels;
  }

  public static function table($model) {
    return Model::parameter($model, 'table');
  }

  public static function isProfile($model) {
    return Model::parameter($model, 'profile_id') > 0;
  }

  public static function asBits($model) {
    $model = self::name($model);
    $field = self::fieldName($model);
    $h = S('dbo')->asHash('SELECT 1 << (id - 1), '.$field
                            .' FROM '.T($model)
                            .' LIMIT 64');
    return $h;
  }

  public static function fieldName($model) {
    $model = self::name($model);
    $field = 'id';
    $fields = self::parameter($model, 'fields');
    foreach (array('name', 'title', 'label', 'code') as $value) {
      if (in_array($value, $fields)) {
        $field = $value;
        break ;
      }
    }
    return $field;
  }
  
  public static function headings($model, $online=false) {
    $where = $online ? ' AND status=1' : '';
    return S('dbo')->asHash('SELECT id, name'
                            .' FROM '.T('heading')
                            .' WHERE model_id='.self::id($model).$where
                            .' ORDER BY position, name');
  }

  public static function asHash($model, $column=null) {
    $model = self::name($model);
    $field = $column ? $column : self::fieldName($model);
    return S('dbo')->query('SELECT id, '.$field
                           .' FROM '.self::table($model)
                           .' ORDER BY '.$field)->asHash();
  }
  
  public static function extension($model) {
    return self::parameter($model, 'extension');
  }

  public static function removeFlag($model, $column, $value) {
    $value = $value instanceof ActiveRecord
           ? NH::asBit($value->getId())
           : intval($value);
    $model = self::name($model);
    S('dbo')->exec('UPDATE '.T($model)
                   . ' SET '.$column.' = '.$column.' & ~'.$value
                   .' WHERE ('.$column.' & '.$value.') > 0');
  }

  public static function getEmptyCollection($model) {
    return new Collection($model, array('filters' => array('sql' => '0')));
  }

  public static function hasProperty($model, $property) {
    $properties = (array) self::parameter($model, 'properties');
    return in_array($property, $properties);
  }

  public static function hasProperties($model) {
    $properties = self::parameter($model, 'properties');
    return !empty($properties);
  }

  public static function properties($item) {
    $model_id = $item->getModelId();
    if (self::$properties[$model_id] === null) {
      self::$properties[$model_id] = S('dbo')->asHash('SELECT *'
                                                      .' FROM '.T('property')
                                                      .' WHERE model_id='.$model_id,
                                                      $null, 'name');
    }
    return self::$properties[$model_id];
  }

  public static function property($item, $name) {
    if (is_numeric($name)) return $name;
    $hash = self::properties($item);
    $ret = $hash[$name]['id'];
    if ($ret < 1) {
      throw new Exception('item property unknown');
    }
    return $ret;
  }

  public static function count($model) {
    return S('dbo')->fetchValue('SELECT count(*)'
                                .' FROM '.T($model));
  }

  public static function exists($model) {
    if (empty($model)) return false;
    if (is_numeric($model)) {
      return array_key_exists((int) $model,
                              $GLOBALS['models']);
    }
    /*    $model = is_numeric($model) ? self::name($model) : $model;*/
    $model = strtolower($model);
    return in_array($model, $GLOBALS['models']);
  }

  public static function max($model, $field='id') {
    return S('dbo')->fetchValue('SELECT MAX('.$field.')'
                                .' FROM '.T($model));
  }

  public static function parameters($model) {
    $model = self::name($model);
    if (!isset(self::$parameters[$model])) {
      $parameters = array();
      include INC_PATH.'/'.$model.'.conf.inc';
      self::$parameters[$model] = $parameters;
    }
    return self::$parameters[$model];
  }

  public static function parameter($model, $name) {
    $parameters = self::parameters($model);
    return $parameters[$name];
  }

  public static function relation($m1, $m2) {
    $m1 = self::name($m1);
    $m2 = self::name($m2);
    foreach (array('belongs_to', 'has_one', 'has_many') as $relation) {
      $h = self::parameter($m1, $relation);
      if ($h[$m2]) return $h[$m2];
    }
    return null;
  }

  public static function singularize($str) {
    $suffix = substr($str, -4);
    if (substr($suffix, 1) === 'ies') {
      $tmp = substr($str, 0, -1);
      if (Model::exists($tmp)) {
        return $tmp;
      }
      $str = rtrim($str, 'ies').'y';
    }
    elseif ($suffix === 'shes') {
      $str = rtrim($str, 'es');
    }
    elseif (substr($str, -1) == 'x') {
      $str = rtrim($str, 'x');
    }
    else {
      $str = rtrim($str, 's');
    }
    return $str;
  }

  public static function models($model) {
    $model = self::name($model);
    $rs = S('dbo')->query('SELECT DISTINCT model_id FROM '.T($model));
    $hash = array();
    foreach ($rs as $row) {
      $model_id = $row['model_id'];
      $hash[$model_id] = Model::name($model_id);
    }
    return $hash;
  }

  public static function reset() {
    self::$parameters = array();
    self::$properties = array();
  }

}
