<?php

class Cetrel {

  private static function config() {
    $config = P('shop.psp');
    $config = $config['cetrel'];
    return $config;
  }

}