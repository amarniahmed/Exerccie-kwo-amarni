<?php

try {

  $filters = array('status' => Mailing::STATUS_SCHEDULED,
                   '|<=|scheduled_at' => DH::now());
  $opts = array('filters' => $filters);
  $mailings = Mailing::collect($opts);
  foreach ($mailings as $mailing) {
    try {
      $campaign = Campaign::getInstanceByItem($mailing);
      $campaign->setStatus(Campaign::STATUS_SCHEDULED);
      $campaign->send();
      S('log')->trace('sending : '.$mailing->name);
    }
    catch (Exception $e) {
      $log->error($e);
    }
    $mailing->setStatus(Mailing::STATUS_DELIVERED);
    break ;
  }

}
catch (Exception $e) { $err->add($e); }
