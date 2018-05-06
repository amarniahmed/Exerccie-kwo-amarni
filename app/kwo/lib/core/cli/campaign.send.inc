<?php

$filters = array('status' => Campaign::STATUS_SCHEDULED,
                 'model_id' => 0,
                 '|<=|scheduled_at' => DH::now());
$opts = array('filters' => $filters);

$campaigns = Campaign::collect($opts);

foreach ($campaigns as $campaign) {
  $campaign->send();
  sleep(2);
}

