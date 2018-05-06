<?php

try { 

  $params = $req->getAttributes();

  $finder = Finder::getInstance($params);

//  $finder->onFind();
  
  if ($req->page > 0) {
    $collection = $finder->getCollection();
    if ($collection && $collection->foundRows() > 0 && count($collection) < 1) {
      $collection->setOffset(0);
    }
  }

  $res->write($finder->asHtml());

}
catch (Exception $e) { $err->add($e); }