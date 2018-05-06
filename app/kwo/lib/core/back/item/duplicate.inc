<?php

try { 

  //$req->debug();

  $item = Item::getInstance(MODEL, $req->id);

  $item = $item->duplicate();

  $ctx->is_node = TreeManager::isNode($item);

  $ctx->id = $item->getId(); 

}
catch (DuplicateRecordException $e) { $err->add('duplication error'); }
catch (Exception $e) { $err->add($e); }