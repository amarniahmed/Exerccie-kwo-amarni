<?php

try {
  
  if ($req->hasAttribute('items', true)) {
    $items = array();
    $set = explode(',', $req->items);
    foreach ($set as $item) {
      $items[] = Item::getInstanceByKey($item)->asPickerHash();
    }
    $ctx->items = $items;
    return ;
  }
  
  if ($req->hasAttribute('item', true)) {
    $item = Item::getInstanceByKey($req->item);
  }
  else {
    $item = Item::getInstance((int) $req->model_id, 
                              (int) $req->record_id);
  }
  $ctx->items = array($item->asPickerHash());

}
catch (Exception $e) { $err->add($e); }


