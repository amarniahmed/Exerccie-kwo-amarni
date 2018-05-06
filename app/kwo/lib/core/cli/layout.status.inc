<?php

try {

  $filters = array('status' => Layout::STATUS_SCHEDULED,
                   '|<=|online_from' => DH::now());
  $opts = array('filters' => $filters);
  foreach (Layout::collect($opts) as $layout) {
    $layout->setStatus(Layout::STATUS_ONLINE);
  }

  $filters = array('status' => Layout::STATUS_ONLINE,
                   '|<=|online_to' => DH::now());
  $opts = array('filters' => $filters);
  foreach (Layout::collect($opts) as $layout) {
    $layout->setStatus(Layout::STATUS_OFFLINE);
  }

}
catch (Exception $e) { $err->add($e); }

