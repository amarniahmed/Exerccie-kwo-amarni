<?php

try {

  $res->setContentType('rss');

  $item = Item::getInstanceByKey($req->item);
  
  $entries = $item->getComments(array('last' => 10));
  
  $res->write($item->asFeed($entries));

}
catch (Exception $e) { $err->add($e); }
