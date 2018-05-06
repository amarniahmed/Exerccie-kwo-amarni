<?php

try {

  if ($req->hasAttribute('item', true)) {
    $item = Item::getInstanceByKey($req->item); 
  }
  else {
    $item = Url::asItem($req->url);
  }

  if (!$item->isOnline()) {
    throw new MaliciousException();
  }
  
  $ctx->item = $item; 
  $ctx->mode = $req->mode;
  $ctx->services = Notification::$services;

  $res->useTemplate();

}
catch (Exception $e) { }