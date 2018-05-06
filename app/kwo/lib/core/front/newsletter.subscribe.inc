<?php

try {

  $email = $req->email;

  if (!Email::isValid($email)) {
    throw new Exception(l('error.email.invalid'));
  }

  $base = Item::getInstanceByKey($req->item);

  if ($base->hasEmail($email)) {
    throw new Exception(l('error.email.subscribed'));
  }

  try {
    $base->addEmail($email, true);
    $item = Email::getInstance(array('email' => $email));
    if ($item->getMeta(Meta::FIELD_VISIT) < 1) {
      $item->setVisit($req);
    }
    if ($item->locale < 1) {
      $item->setAttribute('locale', $req->getLocale());
    }
    Conversion::add($item);    
  }
  catch (DuplicateRecordException $e) {}

  $msg = empty($base->confirmation)
       ? l('subscription.confirm')
       : $base->confirmation;

  $res->sendMessage($msg);

}
catch (Exception $e) {
  $req->track('newsletter signup', Tracker::CATEGORY_ERROR);
  if (strpos($req->email, '@') > 1) {
    $log->error('invalid email address ['.$req->email.']');
  }
  $err->add($e);
}