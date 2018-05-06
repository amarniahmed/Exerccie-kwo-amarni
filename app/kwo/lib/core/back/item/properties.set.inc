<?php

try { 

  $item = Item::getInstance(MODEL, $req->id);

  $properties = array();
  $opts = array('filters' => array('model_id' => $item->getModelId()));
  foreach (Property::collect($opts) as $property) {
    $properties[$property->getId()] = $property;
  }

  /*  $types = $dbo->asHash('SELECT id, type'
                        .' FROM '.T('property')
                        .' WHERE model_id='.$item->getModelId(), 
                        null, 'id', 'type');*/

  foreach ($req->properties as $locale => $hash) {
    $rows = array();
    foreach ($hash as $property_id => $value) {
      if ($properties[$property_id]->hasFlag(Property::TYPE_HTML)) {
        $value = SH::tidy($value);
      }
      $value = SH::texturize($value);
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
catch (Exception $e) { $err->add($e); }