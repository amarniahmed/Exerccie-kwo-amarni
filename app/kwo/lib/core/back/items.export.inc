<?php

try {

  $model = $req->model ?: 'string';
  $model_id = Model::id($model);

  $cache = array('props' => array(), 'datas' => array());
  $id2prop = (array) Model::parameter($model, 'properties');
  if (count($id2prop) > 0) {
    $id2prop = array_flip($id2prop);
    $rs = $dbo->query('SELECT record_id, property_id, locale, value, updated_at'
                      .' FROM '.Item::TABLE_PROPERTY
                      .' WHERE model_id='.$model_id
                      .' ORDER BY record_id ASC, locale ASC, property_id ASC');
    foreach ($rs as $row) {
      $record_id = $row['record_id'];
      $property_id = $row['property_id'];
      $locale = $row['locale'];
      $cache['props'][$record_id][$locale][$property_id] = array('value' => $row['value'],
                                                                 'updated_at' => $row['updated_at']);
    }
  }

  $rs = $dbo->query('SELECT record_id, type, value'
                    .' FROM '.Item::TABLE_DATA
                    .' WHERE model_id='.$model_id
                    .' ORDER BY record_id ASC, type ASC');
  foreach ($rs as $row) {
    $record_id = $row['record_id'];
    $type = $row['type'];
    $cache['datas'][$record_id][$type] = $row['value'];
  }

  $doc = new DOMDocument('1.0', 'UTF-8');
  $doc->xmlStandalone = true;
  $doc->formatOutput = true;
  $items = $doc->createElement('items');
  $items->setAttribute('model', $model);
  $items->setAttribute('model_id', $model_id);
  $items->setAttribute('date', DH::now());
  $items = $doc->appendChild($items);

  $rs = $dbo->query('SELECT * FROM '.T($model).' ORDER BY id ASC');
  $items->setAttribute('count', $rs->count());
  foreach ($rs as $row) {
    $item = $doc->createElement('item');
    $item->setAttribute('id', $row['id']);
    $item->setAttribute('code', $row['code']);
    //-- attributes
    $attrs = $doc->createElement('attributes');
    foreach ($row as $key => $value) {
      $attr = $doc->createElement('attribute', XH::encode($value));
      $attr->setAttribute('name', $key);
      $attrs->appendChild($attr);
    }
    $item->appendChild($attrs);
    //-- properties
    if (isset($cache['props'][$row['id']])) {
      $properties = $doc->createElement('properties');
      foreach ($cache['props'][$row['id']] as $locale => $props) {
        foreach ($props as $property_id => $h) {
          $property = $doc->createElement('property', XH::encode($h['value']));
          $property->setAttribute('name', $id2prop[$property_id]);
          $property->setAttribute('property_id', $property_id);
          $property->setAttribute('locale', $locale);
          $property->setAttribute('updated_at', $h['updated_at']);
          $properties->appendChild($property);
        }
      }
      $item->appendChild($properties);
    }
    //-- datas
    if (isset($cache['datas'][$row['id']])) {
      $datas = $doc->createElement('datas');
      foreach ($cache['datas'][$row['id']] as $type => $value) {
        $data = $doc->createElement('data', XH::encode($value));
        $data->setAttribute('type', $type);
        $datas->appendChild($data);
      }
      $item->appendChild($datas);
    }

    $items->appendChild($item);
//    if ($row['id'] > 5) break ;
  }

  $res->write($doc);

}
catch (Exception $e) { $err->add($e); }
