<?php

try { 

  $ctx->item = Item::getInstance(MODEL);
  
  $res->useXml();

}
catch (Exception $e) { $err->add($e); }