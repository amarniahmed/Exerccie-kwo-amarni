<?php

class Language extends ActiveRecord {

  const STATUS_OFFLINE = -1;
  const STATUS_ONLINE = 1;

  public static $statuses = array(self::STATUS_ONLINE => 'en ligne',
                                  self::STATUS_OFFLINE => 'hors ligne');

  

}