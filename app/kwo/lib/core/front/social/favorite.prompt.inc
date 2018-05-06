<?php

try { 

  $item = Item::getInstanceByKey($req->item);

  $ctx->item = $item;

  $req->track();

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }