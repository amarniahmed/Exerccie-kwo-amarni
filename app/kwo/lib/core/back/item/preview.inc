<?php

try {

  if ($req->hasAttribute('item', true)) {
    $item = Item::getInstanceByKey($req->item);
    $url = $item->asUrl(); 
    $sep = strpos($url, '?') === false ? '?' : '&';
    $url .= $sep.'p=y'; 
  }
  else {
    $url = $req->url;
  }

  $ctx->url = $url;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
