<?php

try { 

  $recipient_id = $user->getInformation('recipient_id');

  if ($recipient_id < 1) {
    $recipient_id = Email::asId($user->email);
    $user->setInformation('recipient_id', $recipient_id);
  }

  $email_id = $recipient_id;

  foreach ($req->bases as $base_id => $flag) {
    $base = Base::getInstance($base_id);
    if (!$base->hasFlag(Base::FLAG_PUBLIC)) continue ; 
    if ($base->hasEmail($email_id)) {
      if ($flag == 0) {
        $base->removeEmail($email_id);
      }
    }
    elseif ($flag == 1) { 
      $base->addEmail($email_id, true);
    }
  }

  $res->setFlash(l('flash.save.success'));

}
catch (Exception $e) { $err->add($e); }
