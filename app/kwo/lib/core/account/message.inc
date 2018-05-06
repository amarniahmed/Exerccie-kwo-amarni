<?php

try {

  $message = Message::getInstance((int) $req->id,
                                  array('include' => array('user'),
                                        'metas' => array('created_at')));

  if (!$message->hasRecipient($user)) {
    throw new Exception('user is not a recipient');
  }

  $message->markAsRead($user);

  $ctx->message = $message;
  if (!$message->hasFlag(Message::FLAG_DIRECT)) {
    $ctx->recipients = $message->recipients();
  }

  $ctx->inbox_offset = $req->inbox_offset;
  $ctx->can_reply = !$message->hasFlag(Message::FLAG_CLOSED);

  $res->setMenu('inbox');
  $res->setTitle(l('account.inbox'));

}
catch (Exception $e) { 
  $log->error($e);
  $err->add($e); 
}