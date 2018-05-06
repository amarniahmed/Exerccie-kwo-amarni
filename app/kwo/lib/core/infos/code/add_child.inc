<?php

try {

  $item = Item::getInstance(MODEL, 
                                          $req->id >= 1 ? $req->id : true);
  
  $child = $item->addChild($req->record);
  
  $ctx->id = $child->getId();

}
catch (Exception $e) { $err->add($e); }