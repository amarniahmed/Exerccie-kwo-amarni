<?php

try { 

  //  $req->debug();

  if ($req->hasAttribute('elements')) {
    $elements = explode(',', $req->elements);
    $i = 1;
    foreach ($elements as $id) {
      $item = Item::getInstance(MODEL, $id);
      $item->commit(array('position' => $i));
      $i++;
    }
    return ;
  }

  $nodes = explode(',', $req->nodes);
  foreach ($nodes as $i => $id) {
    $dbo->update(T(MODEL),
                 array('position' => $i + 1),
                 array('filters' => array('id' => $id),
                       'limit' => 1));
  }

}
catch (Exception $e) { $log->error($e); }