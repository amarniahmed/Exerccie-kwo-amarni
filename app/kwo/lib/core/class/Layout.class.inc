<?php

class Layout extends ActiveRecord {

  const STATUS_ONLINE = 50;
  const STATUS_SCHEDULED = 0;
  const STATUS_OFFLINE = -50;

  public static $statuses = array(self::STATUS_ONLINE => 'online',
                                  self::STATUS_SCHEDULED => 'scheduled', 
                                  self::STATUS_OFFLINE => 'offline');

}