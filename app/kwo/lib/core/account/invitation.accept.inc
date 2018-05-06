<?php

try {

  $invitation = Invitation::getInstance((int) $req->id);
  
  if ($invitation->recipient_model_id != $user->getModelId() ||
      $invitation->recipient_record_id != $user->getId()) {
    throw new MaliciousException();
  }

  $invitation->accept();

  $res->sendMessage(l('invitation.accept.receipt'));

}
catch (Exception $e) { $err->add($e); }
