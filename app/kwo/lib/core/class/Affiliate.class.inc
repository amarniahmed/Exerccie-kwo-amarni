<?php

class Affiliate extends ActiveRecord {

  const STATUS_ONLINE = 10;
  const STATUS_PENDING = 0;
  const STATUS_OFFLINE = -10;

  public static $statuses = array(self::STATUS_ONLINE => 'en ligne',
                                  self::STATUS_PENDING => 'en attente de validation',
                                  self::STATUS_OFFLINE => 'refus');

  const FLAG_WEB = 1;

  public static $flags = array(self::FLAG_WEB => 'origine web');

  public function getDetails() {
    $h = array('interface' => H::url('/affiliate?item='.$this->getKey()));
    return $h;
  }

  public function getConversionCost($conversion) {
    return (float) $this->cost;
  }

}