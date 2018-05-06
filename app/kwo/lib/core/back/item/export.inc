<?php

try {

  if ($req->hasAttribute('finder', true) ||
      $req->hasAttribute('items', true)) {
    $items = BackManager::getBatchCollection();
  }
  else {
    $items = Item::collect(MODEL);
  }

  $filename = $items->getModel() . '_export_' . DH::today();
  $columns = array_keys($items->first()->asExportHash());

  $opts = array('filename' => $filename,
                'columns' => $columns);

  $csv = CsvWriter::getInstance($opts);

  foreach ($items as $item) {
    $csv->append($item->asExportHash());
  }

  $res->sendMessage(c(l('l’opération a bien été réalisée')));

}
catch (Exception $e) { $err->add($e); }