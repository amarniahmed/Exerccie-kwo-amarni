<?php

try {

  $item = Item::getInstanceByKey($req->item);

  if (!$item->isOnline()) {
    throw new MaliciousException();
  }

  if (strlen($req->email) > 5) {
    if (!Email::isValid($req->email, true)) {
      throw new Exception(l('error.email.invalid'));
    }
    $recipient = $req->email;
  }
  elseif ($req->isAuthenticated()) {
    $recipient = $req->getUser();
  }
  else {
    throw new Exception('no recipient');
  }

  $subscription = Subscription::save($item, $recipient);

  if ($subscription instanceof ActiveRecord) {
    if ($req->isAuthenticated()) {
      $subscription->setUser($req->getUser());
    }
    $subscription->setVisit($req);
    $item->incrementMeta(Meta::FIELD_SUBSCRIPTIONS);
    $subscription->convert();    
  }

  $req->track();
  $res->sendMessage('Votre inscription a bien été prise en compte.'); 

}
catch (Exception $e) { 
  $log->error($e);
  $err->add($e); 
}
