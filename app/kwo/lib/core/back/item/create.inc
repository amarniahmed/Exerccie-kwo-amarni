<?php

try { 

  $item = Item::getInstance($req->model);

  $attributes = array('name' => $req->name);
  if (Model::hasField($req->model, 'model_id')) {
    $attributes['model_id'] = Model::id($req->from);
  }

  $item->commit($attributes);

  $ctx->name = (string) $item;
  $ctx->id = $item->getId();

}
catch (Exception $e) { $err->add($e); }
