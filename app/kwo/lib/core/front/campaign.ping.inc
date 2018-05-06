<?php

try {

  $campaign_id = (int) ($req->knxcid > 0 ? $req->knxcid : $req->campaign_id);
  $email_id = (int) ($req->eid > 0 ? $req->eid : $req->email_id);
  
  Act::add(array('email_id' => $email_id,
                 'campaign_id' => $campaign_id,
                 'type' => Act::TYPE_OPENING));
  
  Act::add(array('email_id' => $email_id,
                 'campaign_id' => $campaign_id,
                 'type' => Act::TYPE_SUCCESS));
    
  $email = Email::getInstance($email_id);
  $email->setAttribute('status', $email->status + 2);
  
}
catch (Exception $e) { $log->error($e); }

$res->close();

$res->setContentType('gif');
$res->write(SH::spacer());