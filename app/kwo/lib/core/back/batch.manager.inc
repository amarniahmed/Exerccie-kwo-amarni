<?php

try {

  $model = $req->model;
  
  $items = array();
  if ($req->hasAttribute('items', true)) {
    $items = explode(',', $req->items);
  }
  $items_selected = count($items);

  $finder = Finder::getInstance($req->finder); 

  $items_found = $finder->getCollection()->foundRows();

  $ctx->model = $model;
  $ctx->items_selected = $items_selected;
  $ctx->items_found = $items_found;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
