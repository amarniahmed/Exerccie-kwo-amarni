<?php

try {

  $item = Item::getInstanceByKey($req->item);

  $ctx->name = $item->getName();
  $ctx->item = $item->getKey();

}
catch (Exception $e) { $err->add($e); }
