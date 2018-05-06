<?php

class Settlement extends ActiveRecord {

  const FLAG_UNHEALTHY = 1;

  public static $flags = array(
    self::FLAG_UNHEALTHY => 'insalubre'
  );

  /**
   * Récupère l'adresse complète de la résidence
   * @return string
   */
  public function getAddress() {
    $residence = $this->getResidence();

    return $residence->asAddress();
  }

}
