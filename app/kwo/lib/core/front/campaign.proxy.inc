<?php

try {
  
  $campaign_id = (int) ($req->knxcid ?: $req->campaign_id);
  $email_id = (int) ($req->eid ?: $req->email_id);
  $url_id = (int) ($req->uid ?: $req->url_id);

  $hash = SH::crypt($campaign_id.$email_id);
  
  if ($req->hash !== $hash) {
    throw new Exception('invalid hash');
  }
  
  $url = Url::asString($url_id);
  
  if (empty($url)) {
    throw new Exception('url unknown ('.$url_id.')');
  }
  
  $n = Act::add(array('email_id' => $email_id,
                      'campaign_id' => $campaign_id,
                      'type' => Act::TYPE_REDIRECTION,
                      'value' => $url_id));
  if ($n >= 1) {
    Act::add(array('email_id' => $email_id,
                   'campaign_id' => $campaign_id,
                   'type' => Act::TYPE_VISIT,
                   'value' => $req->getVisitId()));
  }

  Act::add(array('email_id' => $email_id,
                 'campaign_id' => $campaign_id,
                 'type' => Act::TYPE_SUCCESS));
  
  $email = Email::getInstance($email_id);
  $email->setAttribute('status', $email->status + 2);

  $separator = strpos($url, '?') > 6 ? '&' : '?';
  $res->sendRedirect($url.$separator
                     .'email_id='.$email_id.'&campaign_id='.$campaign_id);
}
catch (Exception $e) { 
  $req->debug();
  $log->error($e); 
  $res->sendRedirect('/push/empty');
}

