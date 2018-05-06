<?php

class Currency extends ActiveRecord {

  const FLAG_NO_AUTOUPDATE = 1;

  public static $flags = array(self::FLAG_NO_AUTOUPDATE => 'no update');



  public function getName($locale=null) {
    return $this->name_fr.' ('.$this->code.')';
  }

}