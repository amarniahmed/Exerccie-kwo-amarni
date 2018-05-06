<?php

try {

  $ctx->collection = Item::collect(MODEL);
  
  $res->useXml();
  
}
catch (Exception $e) { $err->add($e); }
