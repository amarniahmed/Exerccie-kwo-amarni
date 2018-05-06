<?php

class String extends ActiveRecord {

  const FLAG_BATCH = 1;

  public static $flags = array(self::FLAG_BATCH => 'autocréation');

  const TYPE_FRONT = 1;
  const TYPE_BACK = 2;

  public static $types = array(self::TYPE_FRONT => 'website',
                               self::TYPE_BACK => 'backoffice');

  const STATUS_DRAFT = 0;
  const STATUS_PENDING = 10;
  const STATUS_ONLINE = 20;

  public static $statuses = array(self::STATUS_ONLINE => 'en ligne',
                                  self::STATUS_PENDING => 'en ligne, à revoir',
                                  self::STATUS_DRAFT => 'brouillon');

  public static $colors = array(self::STATUS_ONLINE => 'green',
                                self::STATUS_PENDING => 'blue',
                                self::STATUS_DRAFT => 'red');


  protected function onBeforeCommit(&$attrs) {
    if (isset($attrs['code'])) {
      $attrs['name'] = $attrs['code'];
    }
  }

  public function asEditHash() {
    if ($this->isItem()) {
      if ($this->type == self::TYPE_BACK) {
        $h['is_back'] = true;
      }
    }
    return $h;
  }

  public function onAfterCommit($hash) {
    if ($this->type == self::TYPE_BACK && !empty(P('back.admin.locales'))) {
      self::CacheWriteType(self::TYPE_BACK);
    }
  }

  public static function apps() {
    $rs = S('dbo')->query('SELECT DISTINCT app FROM '.T('string').' WHERE LENGTH(app)>0');
    $h = array();
    foreach ($rs as $row) {
      $h[$row['app']] = $row['app'];
    }
    asort($h);
    return $h;
  }

  public static function CacheWrite() {
    self::CacheWriteType(self::TYPE_FRONT);
    self::CacheWriteType(self::TYPE_BACK);
    /*$rs = S('dbo')->query('SELECT p.value, p.locale, s.code, s.app'
                          .' FROM '.Item::TABLE_PROPERTY.' AS p,'
                          .     ' '.T('string').' AS s'
                          .' WHERE p.model_id='.Model::id('string')
                          .  ' AND p.record_id=s.id'
                          .  ' AND s.status>='.self::STATUS_DRAFT
                          .' ORDER BY s.code ASC');

    $path = GetText::PATH;
    if (!is_dir($path)) {
      mkdir($path);
    }
    $strings = array();
    foreach ($rs as $row) {
      $strings[$row['locale']] = (array) $strings[$row['locale']];
      $strings[$row['locale']][$row['code']] = $row['value'];
    }
    $words = array();
    $filenames = array();
    foreach ($strings as $locale => $words) {
      $filename = 'messages_'.Locale::toCode($locale).'.inc';//$filename = 'messages_'.I18N::locale2code($locale).'.inc';
      $filenames[] = $filename;
      $file = $path.'/'.$filename;
      file_put_contents($file,
                        '<?php'."\n\n".
                        '$strings = '.var_export($words, true).';'.
                        "\n");
    }
    $files = glob($path.'/*.inc');
    $files = $files ?: array();
    foreach ($files as $file) {
      $filename = basename($file);
      if (in_array($filename, $filenames)) continue ;
      if (file_exists($file)) {
        S('log')->trace('unlink : '.$file, __METHOD__);
        unlink($file);
      }
    }*/
  }

  public static function CacheWriteType($type=self::TYPE_FRONT) {
    $rs = S('dbo')->query('SELECT p.value, p.locale, s.code, s.app'
                          .' FROM '.Item::TABLE_PROPERTY.' AS p,'
                          .     ' '.T('string').' AS s'
                          .' WHERE p.model_id='.Model::id('string')
                          .  ' AND p.record_id=s.id'
                          .  ' AND s.status>='.self::STATUS_DRAFT
                          .  ' AND s.type='.intval($type)
                          .' ORDER BY s.code ASC');
    $path = GetText::PATH;
    if (!is_dir($path)) {
      mkdir($path);
    }
    $strings = array();
    foreach ($rs as $row) {
      if ($type == self::TYPE_BACK
          && !in_array($row['locale'], (array) P('back.admin.locales'))) continue ;
      $strings[$row['locale']] = (array) $strings[$row['locale']];
      $strings[$row['locale']][$row['code']] = $row['value'];
    }
    $words = array();
    $filenames = array();
    $scope = ($type == self::TYPE_BACK) ? 'back_' : '';

    foreach ($strings as $locale => $words) {
      $filename = 'messages_'.$scope.Locale::toCode($locale).'.inc';
      $filenames[] = $filename;
      $file = $path.'/'.$filename;
      file_put_contents($file,
                        '<?php'."\n\n".
                        '$strings = '.var_export($words, true).';'.
                        "\n");
    }
    $files = glob($path.'/'.'messages_'.$scope.'[a-z][a-z].inc');
    $files = $files ? $files : array();
    foreach ($files as $file) {
      $filename = basename($file);
      if (in_array($filename, $filenames)) continue ;
      if (file_exists($file)) {
        S('log')->trace('unlink : '.$file, __METHOD__);
        unlink($file);
      }
    }
    if ($type == self::TYPE_BACK) {
      GetText::reloadBackDict();
    }
  }

}