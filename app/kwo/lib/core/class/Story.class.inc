<?php

class Story extends ActiveRecord {

  const FLAG_NEW = 1;
  const FLAG_LEAD = 2;

  public static $flags = array(self::FLAG_NEW => 'nouveauté',
                               self::FLAG_LEAD => 'à la une');

  const STATUS_CANCELED = -100;
  const STATUS_DRAFT = -50;
  const STATUS_PENDING = -20;
  const STATUS_SCHEDULED = -10;
  const STATUS_PUBLISHED = 10;

  public static $statuses = array(self::STATUS_PUBLISHED => 'publié',
                                  self::STATUS_SCHEDULED => 'planifié',
                                  self::STATUS_PENDING => 'relecture',
                                  self::STATUS_DRAFT => 'brouillon',
                                  self::STATUS_CANCELED => 'annulé');

  public static $colors = array(self::STATUS_PUBLISHED => 'green',
                                self::STATUS_SCHEDULED => 'blue',
                                self::STATUS_PENDING => 'yellow',
                                self::STATUS_DRAFT => 'orange',
                                self::STATUS_CANCELED => 'red');

  public function onBeforeCommit(&$attrs) {
    if (isset($attrs['content'])) {
      $attrs['content'] = SH::tidy($attrs['content']);
    }
    if (isset($attrs['description'])) {
      $attrs['description'] = SH::tidy($attrs['description']);
    }
  }

  public function asDocumentHash($locale) {
    $h = parent::asDocumentHash($locale);
    $h['name'] = $this->name;
    $opts = array('order' => array('position', 'name'));
    foreach ($this->getChapters($opts) as $chapter) {
      $h['content'] .= '<h3>'.$chapter->name.'</h3>';
      $h['content'] .= $chapter->content;
    }
    return $h;
  }

  protected function onAfterStatusChange($old) { 
    if ($this->status == self::STATUS_PUBLISHED && 
        DH::isNull($this->published_at)) {
      $this->setAttribute('published_at', DH::now());
    }
  }

}