<?php

try { 

  $item = Item::getInstance(MODEL, $req->id);
  
  $item->removeItem(Item::getInstanceByKey($req->item_key));

}
catch (Exception $e) { $err->add($e); }