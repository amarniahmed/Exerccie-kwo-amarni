<?php

try {

  $item = Item::getInstance(MODEL, $req->id);
  $metrics = Metrics::getInstance($item, $req->period);
  $metrics->loadDefaultBlocks();

  if (method_exists($item, 'addMetrics')) {
    $item->addMetrics($metrics);
  }

  $res->write($metrics);

}
catch (Exception $e) { $err->add($e); }
