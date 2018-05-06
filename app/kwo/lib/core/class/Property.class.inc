<?php

class Property extends ActiveRecord {

  const TABLE = 'core_property';
  
  const FLAG_NO_TRANSLATION = 1;

  public static $flags = array(self::FLAG_NO_TRANSLATION => 'pas de traduction');

  protected function onBeforeCommit(&$attrs) {
    if (!$this->isItem() && empty($attrs['name'])) {
      throw new ActiveRecordException('nom obligatoire');
    }
    if (isset($attrs['name']) && is_numeric($attrs['name'])) {
      throw new ActiveRecordException('invalid name');
    }
    $table = S('dbo')->getTable(T($attrs['model_id']));
    $table->reset();
    if ($table->hasField($attrs['name'])) {
      throw new ActiveRecordException('une property ne peut avoir le même nom '.
                                      'qu’un attribut ['.$attrs['name'].']');
    }
  }
  
  protected function onAlter() {
    ConfigManager::model(Model::name($this->model_id));
  }
  
  protected function onAfterDestroy() {
    $this->dbo->exec('DELETE FROM '.Item::TABLE_PROPERTY
                     .    ' WHERE property_id='.$this->getId());
  }
 
  public static function models() {
    $models = array();
    foreach (Model::all() as $id => $name) {
      if (!Model::hasField($name, 'locales')) continue ;
      $models[$id] = $name;
    }
    asort($models);
    return $models;
  }

}