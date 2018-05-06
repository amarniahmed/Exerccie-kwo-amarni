<?php

/*****
attention:
- Property : vérification existence de la colonne => modification de la méthode reset de Table
- Model::reset() dans ConfigManager::model()
*****/

try {

  $text_fields = array('body', 'content', 'description', 'footer', 'header',
                       'label', 'lede', 'receipt', 'button',
                       'message', 'precision', 'slug', 'subject', 'title', 'url');

  $model = $req->model;
  if (!Model::exists($model)) {
    throw new Exception('invalid model');
  }
  
  $model_id = Model::id($model);
  $table = T($model);

  if ($req->hasAttribute('fields')) {
    $fields = explode(',', $req->fields);
    foreach ($fields as $field) {
      if (!Model::hasField($model, $field)) {
        throw new Exception('field ['.$field.'] does not exist.');
      }
    }
  }
  else {
    $fields = array();
    foreach ($dbo->getTable($table)->getFields() as $field) {
      if (!in_array($field, $text_fields)) continue ;
      $fields[] = $field;
    }
  }
  
  print_r($fields);

  if (!Shell::confirm('continue', true)) return ;

  if (!$dbo->getTable($table)->hasField('locale')) {
    throw new Exception('Please add a field [locale] in the table ['.$table.'].');
  }

  $col = Model::hasField($model, 'code') ? 'code' : 'id';
  $rs = $dbo->query('SELECT id, '.$col.', locale FROM '.$table.' ORDER BY '.$col.', locale'); 
  $items = array();
  foreach ($rs as $row) {
    if (!$items[$row[$col]]) $items[$row[$col]] = array();
    $locale = $row['locale'] > 0 ? $row['locale'] : 1;
    $items[$row[$col]][$locale] = $row['id'];
  }

  print_r($items);

//  return ;

  $dbo->exec('ALTER TABLE `'.$table.'` CHANGE `locale` `locales` INT UNSIGNED NOT NULL DEFAULT "0"');
  ConfigManager::model($model);

//  $properties = array();
  $attrs = array('model_id' => $model_id);
  foreach ($fields as $field) {
    $dbo->exec('ALTER TABLE `'.$table.'` CHANGE `'.$field.'` `'.$field.'2` TEXT CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL'); 
    ConfigManager::model($model);
    $attrs['name'] = $field;
    $property_id = Property::id($attrs);
    if ($property_id >= 1) continue ;
    $property = Property::getInstance();
    $property->commit($attrs);
    //$property_id = $property->getId();
    //    $properties[$field] = $property_id;
  }

  ConfigManager::model($model);

  foreach ($items as $code => $locales) {
    $i = 1;
    $master = null;
    foreach ($locales as $locale => $id) {
      $item = Item::getInstance($model, $id);
      if ($i == 1) {
        $master = $item;
        $dbo->exec('UPDATE '.Item::TABLE_SEO
                   . ' SET locale='.$locale
                   .' WHERE model_id='.$model_id
                   .  ' AND record_id='.$id
                   .  ' AND locale=0'
                   .' LIMIT 1');
      }
      foreach ($fields as $field) {
        $value = $item->getAttribute($field.'2');
        $master->setProperty($field, $value, $locale);
        $master->setFlag($locale, 'locales');
      }
      if ($i > 1) {
        $dbo->exec('UPDATE '.Item::TABLE_SEO
                   . ' SET locale='.$locale.','
                   .     ' record_id='.$master->id
                   .' WHERE model_id='.$model_id
                   .  ' AND record_id='.$id
                   .' LIMIT 1');
        $dbo->exec('UPDATE '.Item::TABLE_TAG
                   . ' SET locale='.$locale.','
                   .     ' record_id='.$master->id
                   .' WHERE model_id='.$model_id
                   .  ' AND record_id='.$id);
        $item->destroy();
      }
      $i++;
    }
  }

  foreach ($fields as $field) {
    $dbo->exec('ALTER TABLE `'.$table.'` DROP `'.$field.'2`');
  }

  ConfigManager::model($model);

}
catch (Exception $e) { 
  $log->error($e);
}
