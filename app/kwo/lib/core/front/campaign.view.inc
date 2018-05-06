<?php

$res->close();

try {
  
  $campaign_id = (int) ($req->knxcid > 0 ? $req->knxcid : $req->campaign_id);
  $email_id = (int) ($req->eid > 0 ? $req->eid : $req->email_id);

  $hash = SH::crypt($campaign_id . $email_id);

  if ($req->hash !== $hash) {
    $log->error($hash, 'bad hash');
    $req->debug();
    throw new Exception();
  }

  $campaign = Campaign::getInstance($campaign_id);
  
  $content = $campaign->getHtmlContent();
  
  if ($email_id >= 1) {
    $email = Email::getInstance($email_id);
    $tags = array();
    $tags['email_id'] = $email->getId();
    $tags['campaign_id'] = $campaign->getId();
    $tags['email'] = $email->email;
    if (!empty($email->datas)) {
      $tags += (array) unserialize($email->datas);
    }
    $tags['hash'] = $req->hash;
    $tags['link'] = "cliquez ici";
    $tags['remove'] = "cliquez ici";
    $tags['open'] = "";
    $bindings = array();
    foreach ($tags as $key => $value) {
      $bindings['(('.$key.'))'] = $value;
    }
    
    $content = str_ireplace(array_keys($bindings), 
                            array_values($bindings),
                            $content);

    $email->setAttribute('status', $email->status + 2);

  }
  
  $res->write($content);
  
}
catch (Exception $e) {  }