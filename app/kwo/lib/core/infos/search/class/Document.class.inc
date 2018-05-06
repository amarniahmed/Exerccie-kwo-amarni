<?php

// boosting_factor, title, content, keywords, url, record_id, model_id, locale, created_at, updated_at

class Document
{
  private $hash = array();

  public function __construct($hash) {

    if ($hash instanceof ActiveRecord) {
      $icon = '';
      if (strlen($hash->icon) >= 3) {
        $icon = $hash->icon;
      }
      elseif (strlen($hash->image) >= 3) {
        $icon = $hash->image;
      }
      $hash = array('model_id' => $hash->getModelId(),
                    'record_id' => $hash->getId(),
                    'title' => $hash->getTitle(),
                    'description' => $hash->getDescription(),
                    'content' => $hash->getContent(),
                    'icon' => $icon,
                    'keywords' => $hash->getKeywords(),
                    'locale' => $hash->locale,
                    'user_id' => $hash->user_id,
                    'url' => $hash->asUrl(),
                    'updated_at' => $hash->updated_at,
                    'created_at' => $hash->created_at);
    }

    $this->hash = $hash;
    if (is_array($this->keywords)) {
      $this->keywords = join(',', $this->keywords);
    }
    if ($this->record_id < 1) {
      S('log')->error('{'.__METHOD__.'} record_id required');
      throw new Exception('record_id required');
    } 
    if ($this->model_id < 1) {
      S('log')->error('{'.__METHOD__.'} model_id required');
      throw new Exception('model_id required');
    } 
    if (strlen($this->title) < 1) {
      S('log')->error('{'.__METHOD__.'} title required');
      throw new Exception('title required');
    } 
    if ($this->locale < 1) {
      $this->locale = I18N::LOCALE_FR;
    }

    //    S('log')->debug($this->hash);
  }
  
  public function index() {
    self::remove($this->model_id, $this->record_id);
    Indexer::getInstance()->process($this);
  }
  
  private function __get($key)         { return $this->hash[$key]; }
  private function __set($key, $value) { $this->hash[$key] = $value; }
  private function __isset($key)       { return array_key_exists($key, $this->hash); }
  private function __unset($key)       { unset($this->hash[$key]); }

  //-- STATIC ------------------------------------------------------------------

  public static function getInstance() { 
    $class = __CLASS__;
    return new $class; 
  }

  public static function remove($model_id, $record_id=0) {
    if ($model_id instanceof ActiveRecord) {
      $model_id = $model_id->getModelId();
      $record_id = $model_id->getId();
    }
    if ($model_id < 1) return ;
    S('dbo')->exec('DELETE W, D '.
                   'FROM  se_word AS W, se_document AS D '.
                   'WHERE D.record_id=:record_id: AND D.model_id=:model_id: AND '.
                   '      D.record_id=W.record_id AND D.model_id=W.model_id',
                   array (':model_id:' => $model_id,
                          ':record_id:' => $record_id));
  }

}
