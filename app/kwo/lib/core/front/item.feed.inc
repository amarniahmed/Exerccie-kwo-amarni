<?php

try {

  $res->setContentType('rss');

  $item = Item::getInstanceByKey($req->item);
  
  $res->write($item->asFeed());

}
catch (Exception $e) { $err->add($e); }
