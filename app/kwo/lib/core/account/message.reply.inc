<?php

try {

  $content = $req->content;
  $content = SH::clean($content, array('strict' => true));

  if (strlen($content) < 1) {
    throw new Exception(l('error.content.empty'));
  }

  if (strlen($content) > 20000) {
    throw new Exception(l('error.content.size'));
  }

  $message = Item::getInstanceByKey($req->item,
                                    array('metas' => 'bits'));

  if (!$message->isA('message')) {
    throw new MaliciousException();
  }

  if ($message->hasBit(Meta::BIT_LOCKED)) {
    throw new Exception(l('error.item.locked'));
  }

  if ($message->hasFlag(Message::FLAG_DIRECT)) {
    if ($message->user_id == $user->getId()) {
      throw new Exception(l('error.operation.forbidden'));
    }
    $dbo->query('DELETE FROM '.Message::TABLE_RECIPIENTS
                .    ' WHERE message_id='.$message->getId()
                .      ' AND user_id='.$user->getId());
    $recipient = User::getInstance($message->user_id);
    $message = Message::send($user, $recipient,
                             'Re: '.$message->title,
                             $content);
  }
  else {
    $user->comment($message, $content);
  }

  $ctx->id = $message->getId();

}
catch (Exception $e) {
  $log->error($e);
  $err->add($e);
}
