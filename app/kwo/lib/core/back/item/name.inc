<?php

try {

  $item = Item::getInstanceByKey($req->item);
  $name = $item->getName();
  if (empty($name)) {
    $name = ucfirst(Model::name($item)).' ('.$item->getId().')';
  }
  $ctx->name = $name;

}
catch (Exception $e) { $err->add($e); }

