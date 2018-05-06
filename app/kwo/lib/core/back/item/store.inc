<?php

try {

  $id = (int) $req->id;

  $item = Item::getInstance(MODEL, $id ?: true);

  S('app')->hook('onBeforeStore', array('item' => $item));

  if (method_exists($item, 'onBeforeStore')) {
    $item->onBeforeStore();
  }

  $record = $req->record;
  $properties = $req->properties;
  $branch = $req->getBranch();
  $branches = $req->getBranches();

  $required_properties = array();

  if ($req->hasAttribute('xml_file', true)) {
    $doc = new DOMDocument();
    $doc->preserveWhiteSpace = false;
    $doc->load($req->getAttribute('xml_file'));
/*    $xpath = new DOMXPath($doc);
    $query = '//elem';
    $nodes = $xpath->query($query); */
    $nodes = $doc->getElementsByTagName('elem');
    foreach ($nodes as $node) {
      $field = $node->getAttribute('field');
      if (empty($field)) continue ;
      $value = $record[$field];
      if ($node->hasAttribute('required')) {
        if (!Model::hasField($item, $field)) {
          $required_properties[] = $field;
        }
        if ((is_array($value) && count($value) < 1) || (is_string($value) && strlen($value) < 1)) {
          throw new Exception(l('veuillez remplir les champs obligatoires').' ['.$field.']');
        }
      }
      if ($node->hasAttribute('mode') && $node->getAttribute('mode') == 'rich') {
        if (strlen($value) > 5) {
          $value = SH::tidy($value);
          $value = str_replace(array('%28%28', '%29%29'), array('((', '))'), $value);
          $record[$field] = $value;
        }
      }
    }
  }

  if (count($required_properties) > 0) {
    $props = Model::parameter($item, 'properties');
    foreach ($required_properties as &$field) {
      $field = $props[$field];
    }
  }

  if (!$item->isItem()) {
    if (Model::hasOwner($item) && !TreeManager::isNode($item) && count($branch) > 1) {
      /* mluberry */
      if ((isset($record['model_id']) && isset($record['record_id']))
          && ($record['model_id'] < 1 || $record['record_id'] < 1)) {
        throw new Exception(l('veuillez remplir les champs obligatoires').' [item]');
      }
      /* end mluberry */

      if ($req->hasAttribute('caller_item', true)) {
        $dialog = Item::getInstanceByKey($req->getAttribute('caller_item'));
        if (!(isset($record['model_id']) && isset($record['record_id']))) {
          $record['model_id'] = $dialog->getModelId();
          $record['record_id'] = $dialog->getId();
        }
      }
      else {
        $branch_keys = array_keys($branch);
        $model_key = $branch_keys[1];
        list ($model, ) = explode('_', $model_key);
        if (!(isset($record['model_id']) && isset($record['record_id']))) {
          $record['model_id'] = (int) $model;
          $record['record_id'] = $branch[$model_key];
        }
      }
    }

    if (Model::isProfile(MODEL)) {
      $record['user_id'] = $branch['user_id'];
    }
  }

  $belongs_to = (array) Model::parameter(MODEL, 'belongs_to');
  foreach ($belongs_to as $model => $relation) {
    $key = $relation['foreign_key'];
    if ($item->isItem() && isset($record[$key]) && $record[$key] < 1) {
      if ($branch[$key] >= 1) {
        $record[$key] = $branch[$key];
      }
      else {
        throw new Exception(l('veuillez associer un objet de type').' '.
                            '['.strtoupper(Model::label($model)).']');
      }
    }
    elseif (!$item->isItem()) {
      if ($record[$key] < 1) {
        if ($branch[$key] >= 1) {
          $record[$key] = $branch[$key];
        }
        elseif ($branches[$key] >= 1) {
          $record[$key] = $branches[$key];
        }
        else {
          $rels = Model::parameter($model, 'has_many');
          foreach ($rels as $m => $h) {
            if ($m == MODEL) continue ;
            if (isset($h['through'])) continue ;
            if ($branch[$h['foreign_key']] >= 1) {
              try {
                $itm = Item::getInstance($m, (int) $branch[$h['foreign_key']]);
                $record[$key] = $itm->getAttribute($key);
              }
              catch (Exception $e) { $log->error($e); }
            }
          }
        }
      }
      if ($record[$key] < 1) {
        throw new Exception(l('veuillez associer un objet de type').' '.
                            '['.strtoupper(Model::label($model)).']');
      }
    }
  }

  if (isset($record['heading_id']) && $record['heading_id'] < 1) {
    throw new Exception(l('veuillez préciser une rubrique'));
  }

  $item->commit($record);

  if (is_array($properties)) {
    $props = array();
    $opts = array('filters' => array('model_id' => $item->getModelId()));
    foreach (Property::collect($opts) as $property) {
      $props[$property->getId()] = $property;
    }

    foreach ($properties as $locale => $h) {
      foreach ($h as $property_id => $value) {
        if (in_array($property_id, $required_properties) &&
            ((is_array($value) && count($value) < 1) ||
             (is_string($value) && strlen($value) < 1))) {
          //throw new Exception('veuillez remplir le champs obligatoire ['.$props[$property_id]->name.'] ('.I18N::locale2code($locale).')');
          throw new Exception('veuillez remplir le champs obligatoire ['.$props[$property_id]->name.'] ('.Locale::toCode($locale).')');
        }
        if (!($props[$property_id] instanceof ActiveRecord)) {
          S('log')->error('unknown property_id ('.$property_id.')', __METHOD__);
          continue ;
        }
        if (strpos($value, '<') !== false && strpos($value, '>') !== false) {
          $value = SH::tidy($value);
          $value = str_replace(array('%28%28', '%29%29'), array('((', '))'), $value);
        }
        $value = SH::texturize($value);
        $value = trim($value);
        if (in_array($value, array('<br>', '<br />', '<br/>'))) {
          $value = '';
        }
        $rows[$property_id] = $value;
      }
      $item->setProperties($rows, $locale);
    }

    $ope = $locale >= 1 ? ' < 1' : ' >= 1';
    $dbo->exec('DELETE FROM '.Item::TABLE_PROPERTY
               .    ' WHERE locale'.$ope
               .      ' AND model_id='.$item->getModelId()
               .      ' AND record_id='.$item->getId());
  }

  if ($req->hasAttribute('relations')) {
    foreach ($req->relations as $model => $values) {
      $set = $item->getItems($model)->asSet();
      $has_many = Model::parameter($item->getModel(), 'has_many');
      $through = $has_many[$model]['through'];
      if (empty($through)) continue ;
      $values = empty($values) ? array() : $values;
      if ($dbo->getTable($through)->hasField('id')) continue ;
      $item_foreign_key = Model::parameter(MODEL, 'foreign_key');
      $model_foreign_key = Model::parameter($model, 'foreign_key');
      $skip = array();
      foreach ($values as $id) {
        if (in_array($id, $set)) {
          $skip[] = $id;
          continue ;
        }
        $dbo->insert($through,
                     array($item_foreign_key => $item->getId(),
                           $model_foreign_key => $id));
      }
      foreach ($set as $id) {
        if (in_array($id, $skip)) continue ;
        $dbo->exec('DELETE FROM '.$through
                   .' WHERE '.$item_foreign_key.'='.$item->getId()
                   .  ' AND '.$model_foreign_key.'='.$id);
      }
    }
  }

  if ($item->isNew()) {
    $item->setMetas(array(Meta::FIELD_ADMIN => $admin->getId()));
  }

  if ($req->hasAttributes('datas')) {
    foreach ($req->datas as $key => $value) {
      $item->setData(Item::$datas[$key], $value);
    }
  }

  if (method_exists($item, 'onAfterStore')) {
    $item->onAfterStore();
  }

  S('app')->hook('onAfterStore', array('item' => $item));

  $ctx->setAttributes($item);
  $ctx->setAttribute('notify', l('enregistrement réalisé avec succès'));

  $ctx->is_node = TreeManager::isNode($item);
  $ctx->model = $item->getModel();

}
catch (DuplicateRecordException $e) { $err->add(l('error.item.duplicate')); }
catch (Exception $e) {
  $log->error($e);
  $err->add($e);
}