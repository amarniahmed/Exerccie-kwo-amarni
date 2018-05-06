<?php

try { 

  $opts = array('limit' => 20,
                'offset' => intval($req->offset));

  if ($req->hasAttribute('keyword', true)) {
    $opts['filters'] = array('|%%|name' => $req->keyword);
  }

  $items = Item::collect($req->model, $opts);
  
  $ctx->items = $items;

  $res->useTemplate('//item.search');

}
catch (Exception $e) { $err->add($e); }