<?php

class Abuse extends ActiveRecord
{
  const FLAG_ILLICIT = 1;
  const FLAG_ANSWERED = 2;

  public static $flags = array(self::FLAG_ILLICIT => 'illégitime',
                               self::FLAG_ANSWERED => 'défendu');

  const STATUS_PENDING = 0;
  const STATUS_PROCESSING = 1;
  const STATUS_PROCESSED = 2;

  public static $statuses = array(self::STATUS_PROCESSED => 'traité',
                                  self::STATUS_PROCESSING => 'en cours',
                                  self::STATUS_PENDING => 'en attente');

  public function asEditHash() {
    $details['motif'] = $this->getMotive()->getName();
    try {
      $item = $this->getItem();
      $details['objet cible'] = Item::edit($item, array('view' => 'item'));
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
    }
    try {
      $user = $this->getUser();
      $details['origine'] = Item::edit($user);
    }
    catch (Exception $e) { }
    if (!empty($this->full_name)) {
      $details['nom'] = h($this->full_name);
    }
    if (!empty($this->email)) {
      $details['email'] = h($this->email);
    }
    $h['details'] = $details;
    return $h;
  }

  public function onComment($comment) {
    $this->setFlag(self::FLAG_ANSWERED);
  }

  public function asFinderHash($h) {
    $h['content'] = H::ellipse($h['content']);
    return $h;
  }

  public static function onAppReset() {
    $dbo = S('dbo');
    $tables = array(T('abuse'));
    foreach ($tables as $table) {
      if (!$dbo->hasTable($table)) {
        S('log')->trace('table unknown ['.$table.']', __METHOD__);
        continue ;
      }
      $dbo->getTable($table)->truncate();
    }
  }


}