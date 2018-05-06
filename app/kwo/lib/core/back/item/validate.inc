<?php

try {

  $items = BackManager::getBatchCollection();

  $i = 0;
  foreach ($items as $item) {
    try {
      $item->accept();
      $i++;
    }
    catch (Exception $e) { $log->error($e); }
  }

  $res->sendMessage($i.' '.l('élément(s) validé(s)'));

}
catch (Exception $e) { $err->add($e); }