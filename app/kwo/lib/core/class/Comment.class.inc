<?php

class Comment extends ActiveRecord 
{
  const FLAG_SPAM = 1;
  const FLAG_SHADY = 2;
  const FLAG_AGGRESSIVE = 4;
  const FLAG_GOOD = 8;
  const FLAG_BAD = 16;

  public static $flags = array(self::FLAG_SPAM => 'spam',
                               self::FLAG_SHADY => 'suspect',
                               self::FLAG_AGGRESSIVE => 'agressif',
                               self::FLAG_GOOD => 'bon',
                               self::FLAG_BAD => 'mauvais');

  const STATUS_REFUSED = -10;
  const STATUS_PENDING = 1;
  const STATUS_ACCEPTED = 20;

  public static $statuses = array(self::STATUS_ACCEPTED => 'accepté',
                                  self::STATUS_PENDING => 'en attente',
                                  self::STATUS_REFUSED => 'refusé');

  public static $colors = array(self::STATUS_REFUSED => 'red',
                                self::STATUS_PENDING => 'yellow',
                                self::STATUS_ACCEPTED => 'green');

  public function asFinderHash($h) {
    $h['_color'] = self::$colors[$this->status];
    try {
      $item = $this->getItem();
      $attrs = array('href' => 'javascript:void(0)',
                     'onclick' => 'new kwo.dialogs.Preview(this)', 
                     'title' => 'ID '.$item->getId(),
                     'data-url' => $item->asUrl());
      $comment = AH::asTag('a', $attrs) . $item->getName() . '</a> : ';
      $content = strip_tags($this->content);
      $comment .= $content;
      $h['comment'] = $comment;
    }
    catch (Exception $e) { 
      $h['comment'] = '['.$this->model_id.':'.$this->record_id.']';
      $this->destroy();
    }
    return $h;
  }

  public function destroy() {
    $this->setStatus(self::STATUS_REFUSED);
    $this->getItem()->onRelationDestroy($this);
    $this->getItem()->decrementMeta(Meta::FIELD_COMMENTS);
  }

  public function accept() {
    if ($this->status == self::STATUS_ACCEPTED) return ;
    $this->setStatus(self::STATUS_ACCEPTED);
  }

  public static function models() {
    $rs = S('dbo')->query('SELECT DISTINCT model_id'
                          .' FROM '.T('comment')
                          .' WHERE model_id<>'.Model::id('message'));
    $h = array();
    foreach ($rs as $row) {
      $model_id = $row['model_id'];
      $h[$model_id] = Model::name($model_id);
    }
    return $h;
  }

  public static function onAppReset() {
    $dbo = S('dbo');
    $tables = array(T('comment'));
    foreach ($tables as $table) {
      if (!$dbo->hasTable($table)) {
        S('log')->trace('table unknown ['.$table.']', __METHOD__);
        continue ;
      }
      $dbo->getTable($table)->truncate();
    }
  }

}
