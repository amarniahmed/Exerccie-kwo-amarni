<?php

class Translator extends ActiveRecord {

  public function getDetails() {
    $h = array('interface' => H::url('/translator?item='.$this->getKey()));
    return $h;
  }

}