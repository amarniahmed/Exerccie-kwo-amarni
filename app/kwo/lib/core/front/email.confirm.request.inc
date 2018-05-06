<?php

try {

  $user = $req->getUser();

  $user->requestEmailConfirmation();

  $msg = l('email.confirmation.sent', 
           array('email' => $user->email));

  $res->sendMessage(ucfirst($msg).'.');

}
catch (Exception $e) { $err->add($e); }
