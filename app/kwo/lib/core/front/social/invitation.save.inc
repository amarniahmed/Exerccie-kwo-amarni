<?php

try {

  $user = $req->getUser();

  $item = Item::getInstanceByKey($req->item);

  if ($item->isA('user')) {
    if (Network::isConnected($user, $item)) {
      throw new Exception('cet utilisateur fait déjà parti de votre réseau');
    }
  }

  $row = array('model_id' => $item->getModelId(),
               'record_id' => $item->getId(),
               'user_id' => $user->getId());
  
  $recipients = array();

  $friends = (array) $req->friends;
  foreach ($friends as $user_id) {
    if (!Network::isConnected($user, $user_id)) {
      S('log')->trace('security ('.$user->getId().')');
      continue ;
    }
    $recipients[] = array('recipient_model_id' => Model::id('user'),
                          'recipient_record_id' => $user_id);
  }

  if (strlen($req->emails)) {
    $emails = SH::asSet($req->emails);
    foreach ($emails as $email) {
      if (!Email::isValid($email)) continue ;
      $recipients[] = array('recipient_model_id' => Model::id('email'),
                            'recipient_record_id' => Email::asId($email));
    }
  }

  if ($item instanceof User) {
    $recipients[] = array('recipient_model_id' => $item->getModelId(),
                          'recipient_record_id' => $item->getId());
  }

  foreach ($recipients as $recipient) {
    $row['recipient_model_id'] = $recipient['recipient_model_id'];
    $row['recipient_record_id'] = $recipient['recipient_record_id'];
    if ($dbo->getTable(T('invitation'))->hasRow($row)) continue ;
    $invitation = Invitation::getInstance();
    $invitation->commit($row);
    $invitation->send();
  }

  $receipt = Snippet::content('invitation.request.receipt.'.$item->getModel());
  $receipt = empty($receipt) ? 'ok' : $receipt;

  $req->track();
  $item->track(Interaction::TYPE_INVITE);

  $res->sendMessage(ucfirst($receipt));

}
catch (Exception $e) { $err->add($e); }
