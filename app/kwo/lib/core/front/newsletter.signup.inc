<?php

try { 

  $base = Item::getInstanceByKey($req->item);

  $req->track();
  $base->track();

  $ctx->base = $base;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
