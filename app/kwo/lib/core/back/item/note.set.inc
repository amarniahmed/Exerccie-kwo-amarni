<?php

try {

  $item = Item::getInstance(MODEL, (int) $req->id);
  
  $types = array(Item::DATA_NOTE => 'note',
                 Item::DATA_COMMENT => 'comment');

  foreach ($types as $type => $name) {
    $value = $req->getAttribute($name);
    $item->setData($type, $value);
  }
  
}
catch (Exception $e) { $err->add($e); }