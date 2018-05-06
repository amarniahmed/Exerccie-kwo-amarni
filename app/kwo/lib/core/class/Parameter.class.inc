<?php

class Parameter extends ActiveRecord {

  const TYPE_SINGLE  = 1;
  const TYPE_SET     = 2;
  const TYPE_HASH    = 3;

  public static $types = array(self::TYPE_SINGLE => 'simple',
                               self::TYPE_SET => 'set',
                               self::TYPE_HASH => 'hash');
  
  protected function onBeforeCommit(&$attrs) {
    if (isset($attrs['code'])) {
      $attrs['name'] = $attrs['code'];
    }
  }

  protected function onAfterCommit($attrs) {
    ConfigManager::DBCacheBuild();
  }

  protected function onAfterDestroy() {
    ConfigManager::DBCacheBuild();
  }

  public static function asCache() {
    $rs = S('dbo')->query('SELECT code, content, type'
                          .' FROM '.T('parameter')
                          .' WHERE status>0');
    $parameters = array();
    foreach ($rs as $row) {
      $value = $row['content'];
      $type = $row['type'];
      if ($type == self::TYPE_SET) {
        $value = SH::asSet($value);
      }
      elseif ($type == self::TYPE_HASH) {
        $value = SH::asHash($value);
      }
      $parameters[$row['code']] = $value;
    }
    return $parameters;
  }

}