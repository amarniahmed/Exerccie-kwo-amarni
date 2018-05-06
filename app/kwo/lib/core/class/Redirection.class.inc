<?php

class Redirection extends ActiveRecord {

  const FLAG_PERMANENT = 1;

  public static $flags = array(self::FLAG_PERMANENT => 'permanent');

  protected function onBeforeCommit(&$hash) {
    if ($hash['name'] && $hash['destination']) {
      if (substr($hash['destination'], 0, strlen($hash['name'])) === $hash['name']) {
        throw new ActiveRecordException('valeurs invalides, risque de boucle infinie');
      }
    }
  }

  protected function onAfterCommit($hash) {
    ConfigManager::DBCacheBuild();
  }

  protected function onAfterDestroy() {
    ConfigManager::DBCacheBuild();
  }

  public static function asCache() {
    $rs = S('dbo')->query('SELECT destination, flags, name'
                          .' FROM '.T('redirection')
                          .' WHERE status > 0'
                          .' ORDER BY name DESC');
    $hash = array();
    foreach ($rs as $row) {
      $permanent = (bool) ($row['flags'] & self::FLAG_PERMANENT);
      $regex = preg_quote($row['name'], '#');
      $hash[$regex] = array('destination' => $row['destination'],
                            'permanent' => $permanent);
    }
    return $hash;
  }

  public static function asHash() {
    return $GLOBALS['redirections'];
  }

}