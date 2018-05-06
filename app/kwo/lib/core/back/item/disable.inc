<?php

try {

  $item = Item::getInstanceByKey($req->item);
  if (method_exists($item, 'disable')) {
    $item->disable();
  }
  elseif ($item->hasAttribute('status')) {
    $item->setStatus(0);
  }

  $ctx->item = $req->item;

}
catch (Exception $e) { $err->add($e); }
