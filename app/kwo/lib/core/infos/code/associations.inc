<?php

try { 

  $item = Item::getInstance(MODEL, $req->id);

  $model = $req->model;

  $relation = $item->relation($model);

  $through_cols = (array) $relation['through_cols'];
  /*  if (count($through_cols) > 2) {*/
  $n = array_search(MODEL.'_id', $through_cols);
  if ($n !== false) {
    unset($through_cols[$n]);
  }
  $n = array_search($model.'_id', $through_cols);
  if ($n !== false) {
    unset($through_cols[$n]);
  }
    /*  } */

  $ctx->through_cols = array();
  foreach ($through_cols as $col) {
    $ctx->through_cols[$col] = $col;
  }

  $ctx->cols = (array) $relation['cols'];

  $opts = array();
  foreach ($ctx->through_cols as $column) {
    $opts['cols'][$relation['through'].'.'.$column] = $column.'_';
  }

  $items = $item->getItems($model, $opts);

  if ($req->append == 1) {
    $opts = array('filters' => array('|!IN|id' => $items));
    $orphans = Item::collect($model, $opts);
    if ($orphans->count() >= 1) {
      $ctx->orphans = $orphans;
    }
  }

  $ctx->label = ucfirst(M($model, 'label'));
  $ctx->items = $items;

  $res->useTemplates('//associations');

}
catch (Exception $e) { $err->add($e); }
