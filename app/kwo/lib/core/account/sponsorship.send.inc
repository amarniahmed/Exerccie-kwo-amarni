<?php

try {

  if (is_array($req->godsons)) {
    $duplicates = array();
    foreach ($req->godsons as $i => $email) {
      if (User::exists(array('email' => $email))) {
        $duplicates[] = strlen($req->names[$i]) > 0
                      ? $req->names[$i].' <'.$email.'>'
                      : $email;
      }
    }
    if (count($duplicates) > 0) {
      throw new Exception(l('error.godson.duplicate')."\n".
                          implode("\n", $duplicates) );
    } 
    $content = SH::clean($req->content);
    $opts = array('content' => $content);
    foreach ($req->godsons as $i => $email) {
      $opts['name'] = (string) $req->names[$i];
      Sponsorship::deliver($user, $email, $opts);
    }
  }
  elseif (is_array($req->sponsorships)) {
    foreach ($req->sponsorships as $sponsorship_id) {
      $sponsorship = Sponsorship::getInstance($sponsorship_id);
      $sponsorship->send();
    }
  }

  $res->setFlash(l('flash.message.sent'));

}
catch (Exception $e) { $err->add($e); }