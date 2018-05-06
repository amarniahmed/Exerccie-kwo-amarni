<?php

class Psp extends ActiveRecord {

  public static function load($name) {
    $name = (string) $name;
    $name = ucfirst($name);
    require LIB_PATH.'/core/class/psp/'.$name.'.class.inc';
  }

}
