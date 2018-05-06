<?php

try {

  $user = $req->getUser();

  $user_id = (int) $req->user_id;

  if ($user_id < 1) {
    throw new Exception(l('error'));
  }

  $row = array('model_id' => Model::id('user'),
               'record_id' => $user_id,
               'user_id' => $user->getId(),
               'recipient_model_id' => Model::id('user'),
               'recipient_record_id' => $user_id);
  if (!$dbo->getTable(T('invitation'))->hasRow($row)) {
    $invitation = Invitation::getInstance();
    $invitation->commit($row);
  }

  $res->sendMessage('Votre demande a bien été transmise');

}
catch (Exception $e) { $err->add($e); }
