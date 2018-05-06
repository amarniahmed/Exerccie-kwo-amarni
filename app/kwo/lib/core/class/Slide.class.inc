<?php

class Slide extends ActiveRecord {

  const STATUS_ONLINE = 1;
  const STATUS_SCHEDULED = 0;
  const STATUS_OFFLINE = -1;

  public static $statuses = array(self::STATUS_ONLINE => 'en ligne',
                                  self::STATUS_SCHEDULED => 'planifié',
                                  self::STATUS_OFFLINE => 'hors ligne');
  
  protected function onBeforeCreate(&$attrs) {
    if (empty($attrs['position'])) {
      $position = $this->dbo->fetchValue('SELECT MAX(position)'
                                         .' FROM '.T('slide')
                                         .' WHERE slideshow_id='.$attrs['slideshow_id']);
      $attrs['position'] = intval($position) + 1;
    }
  }

}