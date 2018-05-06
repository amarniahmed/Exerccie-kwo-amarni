<?php

try { 
  
  $item = Item::getInstance(MODEL, $req->id);
  $ctx->values = method_exists($item, 'getStats')
               ? $item->getStats()
               : array();
    
  $res->useXml();

}
catch (Exception $e) { $err->add($e); }