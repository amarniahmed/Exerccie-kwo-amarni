<?php

try { 

  $item = Item::getInstance(MODEL, $req->id);

  $item->addItem(Item::getInstanceByKey($req->item_key),
                 $req->attributes);

}
catch (Exception $e) { $err->add($e); }