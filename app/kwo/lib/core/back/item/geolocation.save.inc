<?php

try {

  $item = Item::getInstance(MODEL, (int) $req->id);

  $row = $req->getAttribute('row');

  $id = Geolocation::id(array('model_id' => $item->getModelId(),
                              'record_id' => $item->getId()));

  $row += array('model_id' => $item->getModelId(),
                'record_id' => $item->getId());

  if ($req->hasAttribute('country_code', true)) {
    $row['country_id'] = is_numeric($req->country_code)
                       ? $req->country_code
                       : Country::id(array('code' => $req->country_code));
  }

  $row['name'] = $item->getName();

  $args = $id > 0 ? intval($id) : true;

  $geolocation = Geolocation::getInstance($args);
  $geolocation->commit($row);

  $attrs = $metas = array();

  $metas[Meta::FIELD_LATITUDE] = $row['latitude'];
  $metas[Meta::FIELD_LONGITUDE] = $row['longitude'];

  $fields = array('latitude', 'longitude', 'admin1', 'country_id', 'postal_code', 'radius');
  foreach ($fields as $field) {
    if (!$item->hasAttribute($field)) continue ;
    $attrs[$field] = $row[$field];
  }

  if ($item->hasAttribute('formatted_address')) {
    $attrs['formatted_address'] = $row['address'];
  }
  if ($item->hasAttribute('city')) {
    $attrs['city'] = $row['locality'];
  }

  $item->setMetas($metas);

  if (!empty($attrs)) {
    $item->commit($attrs);
  }

  if (method_exists($item, 'onAfterGeocode')) {
    $item->onAfterGeocode();
  }

}
catch (Exception $e) { $err->add($e); }
