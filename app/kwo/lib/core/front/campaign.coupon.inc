<?php

try {
  
  $campaign_id = (int) ($req->knxcid > 0 ? $req->knxcid : $req->campaign_id);
  $email_id = (int) ($req->eid > 0 ? $req->eid : $req->email_id);
  
  Act::add(array('email_id' => $email_id,
                 'campaign_id' => $campaign_id,
                 'type' => Act::TYPE_COUPON,
                 'value' => $req->coupon));
  die('1');
  
}
catch (Exception $e) {
  die('0');
}
