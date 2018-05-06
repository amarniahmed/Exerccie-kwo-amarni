<?php

try {

  $item = Item::getInstance(MODEL, $req->id);

  $ctx->item = $item;

  $res->useXml();

}
catch (Exception $e) { $err->add($e); }