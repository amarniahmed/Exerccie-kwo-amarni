<?php

class Document
{
  const TABLE = 'core_document';

  const STATUS_ONLINE = 200;

  public $boosting_factor = null;
  public $content = null;
  public $description = null;
  public $item = null;
  public $locale = null;
  public $keywords = null;
  public $model_id = null;
  public $name = null;
  public $record_id = null;
  public $status = null;

  public function __construct(ActiveRecord $item, $locale=0) {
    $this->item = $item;
    $hash = $item->asDocumentHash($locale);
    if (empty($hash)) {
      throw new Exception('invalid : '.
                          $item->getModel().'('.$item->getId().')->asDocumentHash($locale)');
    }
    foreach ($hash as $key => $value) {
      if ($value === null) continue ;
      $this->{$key} = $value;
    }
    $this->locale = $locale;
    $this->keywords = is_array($this->keywords)
                    ? join(',', $this->keywords)
                    : $this->keywords;
    if ($this->record_id < 1 || $this->model_id < 1) {
      S('log')->error('failure : model_id or record_id', __METHOD__);
      throw new Exception('invalid arguments');
    }
  }

  public function debug() {
    $hash = array();
    $fields = array('name', 'content', 'description', 'keywords', 'locale');
    foreach ($fields as $field) {
      $hash[$field] = $this->{$field};
    }
    S('log')->debug($hash, __METHOD__);
  }

  public function asText() {
    $out = '';
    foreach (array('name', 'keywords', 'description', 'content') as $attribute) {
      $str = trim($this->{$attribute});
      if (empty($str)) continue ;
      $out .= $str."\n";
    }
    return strip_tags($out);
  }

  public static function setStatus($item) {
    if ($item instanceof ActiveRecord) {
      $model_id = $item->getModelId();
      $record_id = $item->getId();
    }
    elseif (isset($item['model_id'])) {
      $model_id = (int) $item['model_id'];
      $record_id = (int) $item['record_id'];
      $item = Item::getInstance($model_id, $record_id);
    }
    else {
      $model_id = (int) $item[0];
      $record_id = (int) $item[1];
      $item = Item::getInstance($model_id, $record_id);
    }
    if (!$item->isOnline()) {
      self::remove($item);
      return ;
    }
    $status = 1;
    $opts = array('filters' => array('model_id' => $model_id,
                                     'record_id' => $record_id),
                  'modifiers' => array('ignore'));
    $record = array('status' => $status);
    S('dbo')->update(self::TABLE, $record, $opts);
    if ($status === 1 && S('dbo')->matchedRows() < 1) {
      if (Model::hasAnyField($item, array('locale', 'locales'))) {
        $locales = $item->getLocales();
      }
      else {
        $locales = array(S('app')->getLocale());
      }
      foreach ($locales as $locale) {
        S('dbo')->insert(self::TABLE,
                         array('model_id' => $model_id,
                               'record_id' => $record_id,
                               'locale' => $locale,
                               'status' => $status));
      }
    }
  }

  public static function hasItem($item) {
    if ($item instanceof ActiveRecord) {
      $model_id = $item->getModelId();
      $record_id = $item->getId();
    }
    elseif (isset($item['model_id']) && isset($item['model_id'])) {
      $model_id = (int) $item['model_id'];
      $record_id = (int) $item['record_id'];
    }
    else {
      $model_id = (int) $item[0];
      $record_id = (int) $item[1];
    }
    $row = array('model_id' => $model_id,
                 'record_id' => $record_id);
    return S('dbo')->getTable(self::TABLE)->hasRow($row);
  }

  public static function remove($item, $locale=null) {
    $locale = (int) $locale;
    if ($item instanceof ActiveRecord) {
      $model_id = $item->getModelId();
      $record_id = $item->getId();
    }
    elseif (isset($item['model_id']) && isset($item['model_id'])) {
      $model_id = (int) $item['model_id'];
      $record_id = (int) $item['record_id'];
    }
    else {
      $model_id = (int) $item[0];
      $record_id = (int) $item[1];
    }
    if ($model_id < 1) return ;
    $where = '';
    if ($locale >= 1) {
      $where = ' AND locale='.intval($locale);
    }
    $dbo = S('dbo');
    try {
      $documents = $dbo->asSet('SELECT id'
                               .' FROM '.self::TABLE
                               .' WHERE model_id='.$model_id
                               .  ' AND record_id='.$record_id
                               .$where);
      foreach ($documents as $document_id) {
        $dbo->exec('DELETE FROM '.Engine::TABLE_BUCKET
                   .' WHERE document_id='.$document_id);
        $dbo->exec('DELETE FROM '.Engine::TABLE_INDEX
                   .' WHERE document_id='.$document_id);
        $dbo->exec('DELETE FROM '.self::TABLE
                   .' WHERE id='.$document_id);
      }
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
    }
  }

  public static function onAppReset() {
    $dbo = S('dbo');
    $tables = array(Engine::TABLE_BUCKET, Engine::TABLE_INDEX, Engine::TABLE_STEM,
                    Document::TABLE,
                    T('engine'), T('synonym'));
    foreach ($tables as $table) {
      if (!$dbo->hasTable($table)) {
        S('log')->trace('table unknown ['.$table.']', __METHOD__);
        continue ;
      }
      $dbo->getTable($table)->truncate();
    }
  }

}
