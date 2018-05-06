<?php

try { 

  $messages = (array) $req->messages;

  if (count($messages) < 1) {
    throw new Exception(l('error.messages.empty'));
  }

  $opts = array('filters' => array('message_id' => $messages,
                                   'user_id' => $user->getId()));
  $record = array('status' => 2);
  $dbo->update(Message::TABLE_RECIPIENTS, $record, $opts);

}
catch (Exception $e) { $err->add($e); }
