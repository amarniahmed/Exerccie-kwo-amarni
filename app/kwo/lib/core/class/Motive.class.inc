<?php

class Motive extends ActiveRecord {

  const FLAG_WARN = 1;
  const FLAG_HIDE = 2;
  
  public static $flags = array(self::FLAG_WARN => 'avertir par mail',
                               self::FLAG_HIDE => 'cacher automatiquement');

  //  public static $thresholds = array(1 => )

}