<?php

try {

  $item = Item::getInstanceByKey($req->item);

  if (!$item->isOnline()) {
    throw new MaliciousException();
  }

  $ctx->item = $item;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
