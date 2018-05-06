<?php

try {

  return ; 

  $filters = array('status' => Alert::STATUS_ONLINE);
  $opts = array('filters' => $filters);
  foreach (Search::collect($opts) as $search) {
    $search->refresh();
  }

}
catch (Exception $e) { $err->add($e); }
