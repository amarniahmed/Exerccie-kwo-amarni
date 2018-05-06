<?php

try {

  $res->setContentType('rss');
  
  $alert = Item::getInstanceByKey($req->item);

  $res->write($alert->asFeed());

}
catch (Exception $e) { $err->add($e); }
