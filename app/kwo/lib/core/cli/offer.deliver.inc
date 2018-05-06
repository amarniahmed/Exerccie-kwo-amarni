<?php

try {

  $filters = array('status' => Offer::STATUS_SCHEDULED,
                   '|<|scheduled_at' => DH::now());
  $opts = array('filters' => $filters);
  foreach (Offer::collect($opts) as $offer) {
//    $offer->setStatus(Offer::STATUS_PROCESSING);
    $offer->deliver(false);
  }

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
