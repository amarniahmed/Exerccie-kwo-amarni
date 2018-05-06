<?php

class Announcement extends ActiveRecord {

  const STATUS_DRAFT = 0;
  const STATUS_SENT = 1;

  public static $statuses = array(self::STATUS_DRAFT => 'brouillon',
                                  self::STATUS_SENT => 'envoyé');

  const FLAG_RECIPIENT_USER = 1;
  const FLAG_RECIPIENT_EMAIL = 2;

  public static $flags = array(self::FLAG_RECIPIENT_USER => 'users',
                               self::FLAG_RECIPIENT_EMAIL => 'emails');

  public function deliver() {
    
  }

}