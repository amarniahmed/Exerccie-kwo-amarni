<?php

class Page extends ActiveRecord {

  const TABLE_BLOCK = 'core_block_page';

  const STATUS_DRAFT = 0;
  const STATUS_PENDING = 10;
  const STATUS_ONLINE = 20;
  const STATUS_HIDDEN = -10;
  
  public static $statuses = array(self::STATUS_ONLINE => 'en ligne',
                                  self::STATUS_PENDING => 'en ligne, à revoir',
                                  self::STATUS_DRAFT => 'brouillon',
                                  self::STATUS_HIDDEN => 'cachée');
  
  public static $colors = array(self::STATUS_ONLINE => 'green',
                                self::STATUS_PENDING => 'blue',
                                self::STATUS_DRAFT => 'yellow',
                                self::STATUS_HIDDEN => 'orange');

  public function getDetails() {
    return array('url' => $this->asUrl());
  }

  public function asRelationEditHash($elts, $relation) {
    $elts['status']['label'] = 'état';
    $elts['status']['type'] = 'select';
    $elts['status']['options'] = array(1 => 'en ligne', 
                                       0 => 'caché');
    return $elts;
  }

  public static function apps() {
    $rs = S('dbo')->query('SELECT DISTINCT app FROM '.T('page').' WHERE LENGTH(app)>0');
    $h = array();
    foreach ($rs as $row) {
      $h[$row['app']] = $row['app']; 
    }
    asort($h);
    return $h;
  }

}
