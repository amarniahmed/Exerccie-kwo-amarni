<?php

try {
  
  $log->enable();

  if ($req->proof == 1) {
    die('1');
  }

  if (strlen($req->dsn) != 5) {
    $log->trace('bad dsn ['.$req->dsn.']');
    throw new Exception('bad dsn ['.$req->dsn.']', true);
  }
  
  $dsn = (int) str_replace('.', '', $req->dsn);
  
  if ($dsn === Smtp::CODE_SUCCESS) {
    $type = Act::TYPE_SUCCESS;
    $value = 0;
  }
  elseif (Smtp::isHardbounce($dsn)) {
    $type = Act::TYPE_HARDBOUNCE;
    $value = $dsn - 400;
  }
  else {
    $type = Act::TYPE_SOFTBOUNCE;
    $value = $dsn - 400;
  }
  
  $email_id = 0;
  if ($req->contact_id > 0) {
    $email_id = (int) $req->contact_id;
  }
  elseif ($req->email_id > 0) {
    $email_id = (int) $req->email_id;
  }

  Smtp::add(array('email_id' => $email_id,
                  'campaign_id' => $req->campaign_id,
                  'smtp_key' => $req->smtp_key,
                  'relay_host' => $req->relay_host,
                  'relay_ip' => $req->relay_ip,
                  'stat' => $req->stat,
                  'dsn' => $req->dsn,
                  'from_time' => $req->from_time,
                  'to_time' => $req->to_time));

  $n = Act::add(array('email_id' => $email_id,
                      'campaign_id' => $req->campaign_id,
                      'type' => $type,
                      'value' => $value));

  if ($n >= 1 && $type != Act::TYPE_SUCCESS) {
    $email = Email::getInstance($email_id);
    if ($email->status >= 0) {
      $n = $type == Act::TYPE_HARDBOUNCE ? 4 : 2;
      $email->setAttribute('status', $email->status - $n);
    }
  }
  
  die('1');

}
catch (Exception $e) { die('0'); }
