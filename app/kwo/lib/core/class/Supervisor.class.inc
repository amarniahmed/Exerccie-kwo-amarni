<?php

class Supervisor extends ActiveRecord {

  public static $models = array('user', 'marchandise');

  public static function models() {
    $models = array();
    foreach (self::$models as $model) {
      if (!Model::exist($model)) continue ;
      $models[Model::id($model)] = Model::name($model);
    }
    return $models;
  }

}