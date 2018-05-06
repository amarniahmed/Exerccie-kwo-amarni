<?php

try { 

  $opts = array('filters' => array('|&|flags' => Base::FLAG_PUBLIC,
                                   'locale' => $res->getLocale()),
                'order' => array('position' => 'asc'));
  
  $bases = Base::collect($opts);

  $recipient_id = $user->getInformation('recipient_id');
  if ($recipient_id < 1) {
    $recipient_id = Email::asId($user->email);
    $user->setInformation('recipient_id', $recipient_id);
  }

  $email_id = $recipient_id;

  $email = Email::getInstance($email_id)->email;

  $ctx->email = $email;
  $ctx->email_id = $email_id;
  $ctx->bases = $bases;

  $res->setMenu('newsletters');
  $res->setTitle(l('account.newsletters'));

}
catch (Exception $e) { $err->add($e); }
