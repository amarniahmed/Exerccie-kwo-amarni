<?php

try {

/***
TODO:
- vérifier que les attributs/properties correspondent

***/

  $file = 'web/'.DEVEL.'/files/test.xml';

  $doc = new DOMDocument();
  $doc->preserveWhiteSpace = false;
  $doc->load($file);

  $model = $doc->firstChild->getAttribute('model');
  $model_id = Model::id($model);

  $cache = array();
  
  $rs = $dbo->query('SELECT record_id, property_id, locale, value, updated_at'
                    .' FROM '.Item::TABLE_PROPERTY
                    .' WHERE model_id='.$model_id
                    .' ORDER BY record_id ASC, locale ASC, property_id ASC');
  $i = 0;
  foreach ($rs as $row) {
    $record_id = (int) $row['record_id'];
    $property_id = (int) $row['property_id'];
    $locale = (int) $row['locale'];
    $h = array('value' => $row['value'],
               'updated_at' => $row['updated_at']);
    $cache[$record_id.'-'.$property_id.'-'.$locale] = $h;
    if ($i == 0 && strlen($row['updated_at']) != strlen('0000-00-00 00:00:00')) {
      $log->error('invalid date format');
      throw new Exception('invalid date format');
    }
    $i++;
//    $log->debug($cache);
  }
  

/*  $xpath = new DomXPath($doc);
  $items = $xpath->query('//item'); */

  $nodes = $doc->getElementsByTagName('item');

  foreach ($nodes as $node) {
    $code = $node->getAttribute('code');
    $id = (int) $node->getAttribute('id');
    if (empty($code)) continue ;
    $record_id = Item::id($model, array('code' => $code));
    if ($record_id > 0) {
//      $log->trace($code);
      $item = Item::getInstance($model, $record_id);
      $properties = $node->getElementsByTagName('property');
      foreach ($properties as $property) {
        $name = $property->getAttribute('name');
        $property_id = (int) $property->getAttribute('property_id');
        $locale = (int) $property->getAttribute('locale');
        $updated_at = $property->getAttribute('updated_at');
        $value = $property->textContent;
        $key = $item->getId().'-'.$property_id.'-'.$locale;
//        $log->trace($key);
        if (!empty($value)) {
          if (empty($cache[$key]['value']) && !empty($value)) {
            $log->trace("empty: $code, $name, $locale, $value");
            $item->setProperty($name, $value, $locale);
          }
          elseif (!DH::isNull($updated_at) && $updated_at > $cache[$key]['updated_at']) {
            $log->trace("newer: $code, $name, $locale, $value");
            $item->setProperty($name, $value, $locale);
          }
        }
      }
    }
    else {
      $item = Item::getInstance($model);
      $attrs = array();
      foreach ($node->getElementsByTagName('attribute') as $attr) {
        $name = $attr->getAttribute('name');
        if ($name == 'id') continue ;
        $value = $attr->textContent;
        $attrs[$name] = $value;
      }
      if (empty($attrs)) continue ;
      $item->commit($attrs);
      $properties = $node->getElementsByTagName('property');
      foreach ($properties as $property) {
        $name = $property->getAttribute('name');
        $locale = (int) $property->getAttribute('locale');
        $value = $property->textContent;
        $item->setProperty($name, $value, $locale);
      }
    }
  }

}
catch (Exception $e) { $err->add($e); }
