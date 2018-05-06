<?php

try {

  $item = Item::getInstanceByKey($req->item);
  $html = $item->asView($req->getAttribute('view'));
  if (empty($html)) {
    $html = '** '.c(Model::name($item)).' ('.$item->getId().') **';
  }
  $res->write($html);

}
catch (Exception $e) { $err->add($e); }
