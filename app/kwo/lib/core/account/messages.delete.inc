<?php

try { 
  
  $messages = (array) $req->messages;
  
  if (count($messages) < 1) {
    throw new Exception(l('error.messages.empty'));
  }

  foreach ($messages as $id) {
    $message = Message::getInstance((int) $id);
    $message->delete($user);
  }
  
}
catch (Exception $e) { $err->add($e); }