<?php

try { 

  $req->cleanAttributes();

  $recipients = SH::asSet($req->recipients);

  if (empty($recipients)) {
    throw new Exception(l('recipients.required'));
  }
  
  if (count($recipients) > 20) {
    throw new Exception(l('recipients.toomany'));
  }

  $from = $req->isAuthenticated()
        ? $req->getUser()->email
        : $req->from;
  
  if (!Email::isValid($from)) {
    throw new Exception(l('error.email.invalid'));
  }
  
  $item = Item::getInstanceByKey($req->item); 
  
  $attrs = array('model_id' => $item->getModelId(),
                 'record_id' => $item->getId(),
                 'comment' => '',
                 'type' => Notification::TYPE_MAIL);
  $comment = '';
  if (!empty($req->comment)) {
    $attrs['comment'] = $req->comment;
    $comment = '<br/><em>'.$attrs['comment'].'</em><br/>';
  }

  $notification = Notification::getInstance();
  $notification->commit($attrs);
  $notification->setVisit($req);
  $notification_id = $notification->getId();

  $item->incrementMeta(Meta::FIELD_SHARES); 
  if (method_exists($item, 'onShare')) {
    $item->onShare($favorite);
  }
  
  $params = array();
  if ($req->isAuthenticated() && $req->sponsorship == 1) {
    $params['sponsor_id'] = $req->getUserId();
  }

  $bindings = array('comment' => $comment,
                    'link' => H::link(null, $item, $params));
  $opts = array('from' => $from);
  
  foreach ($recipients as $email) {
    if (!Email::isValid($email)) continue ;
    Letter::send('notification', $email, $bindings, $opts);
    $dbo->insert(Notification::TABLE_EMAIL,
                 array('email' => $email,
                       'notification_id' => $notification_id));
  }
  
  if ($req->send_copy) {
    Letter::send('notification', $from, $bindings, $opts);
  }

  $req->track();

  $res->sendMessage(l('notification.sent'));

}
catch (Exception $e) { $err->add($e); }
