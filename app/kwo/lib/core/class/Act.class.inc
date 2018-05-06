<?php

class Act
{
  const TYPE_SENDING = 1;
  const TYPE_OPENING = 2;
  const TYPE_REDIRECTION = 3;
  const TYPE_VISIT = 4;
  const TYPE_SUCCESS = 5;
  const TYPE_SOFTBOUNCE = 6;
  const TYPE_HARDBOUNCE = 7;
  const TYPE_REMOVE = 20;
  const TYPE_BLACKLIST = 21;

  const TABLE = 'core_act';

  public static function add($row) {
    $dbo = S('dbo');
    $row = (array) $row;
    if ($row['campaign_id'] < 1 || $row['type'] < 1 || $row['email_id'] < 1) {
      S('log')->debug($row, __METHOD__);
      return false;
    }
    if ($row['type'] == self::TYPE_SUCCESS) {
      $dbo->exec('DELETE FROM '.self::TABLE
                 .' WHERE type IN ('.self::TYPE_SOFTBOUNCE.','
                 .                   self::TYPE_HARDBOUNCE.')'
                 .  ' AND campaign_id='.$row['campaign_id']
                 .  ' AND email_id='.$row['email_id']);
    }
    elseif ($row['type'] == self::TYPE_SOFTBOUNCE || 
            $row['type'] == self::TYPE_HARDBOUNCE) {
      $line = array('campaign_id' => $row['campaign_id'],
                    'email_id' => $row['email_id'],
                    'type' => self::TYPE_SUCCESS);
      if ($dbo->getTable(self::TABLE)->hasRow($line)) {
        return 0;
      }
    }
    $dbo->insert(Act::TABLE, $row, array('modifiers' => 'ignore'));
    return $dbo->affectedRows();
  }

  public static function exists($row) {
    
  }

}