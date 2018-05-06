<?php

try { 
  
  $item = Item::getInstanceByKey($req->item);
  
  $motive = Motive::getInstance($req->motive_id);

  $req->cleanAttributes();

  $content = $req->content;
  $full_name = $req->full_name;
  $email = $req->email;

  $attrs = array('model_id' => $item->getModelId(),
                 'record_id' => $item->getId(),
                 'content' => $content,
                 'motive_id' => (int) $req->motive_id,
                 'full_name' => $full_name,
                 'email' => $email);

  $abuse = Abuse::getInstance();
  $abuse->commit($attrs);
  $abuse->setVisit($req);
  
  if ($req->isAuthenticated()) {
    $user = $req->getUser();
    $abuse->setUser($user);
    Letter::send('abuse.report.confirm', $user);
  }
  
  if ($motive->hasFlag(Motive::FLAG_WARN)) {
    $h = array('motive' => $motive->getName(),
               'about' => $content,
               'url' => $req->getClient('referer'),
               'target' => Model::name($item).' ('.$item->getModelId().')');
    foreach (array('name', 'title', 'content') as $field) {
      if (!$item->hasAttribute($field)) continue ;
      $h['target - '.$field] = $item->getAttribute($field);
    }
    $h = array_merge($h, $req->getClient());
    $recipients = empty($motive->recipients) 
                ? P('contact.admin')
                : SH::asSet($motive->recipients);
    $mail = Mail::getInstance();
    $mail->setSubject('abuse report : '.$item->getModel());
    $mail->setBody($h);
    $mail->addRecipient($recipients);
    $mail->send();
  }
  
  if ($motive->hasFlag(Motive::FLAG_HIDE)) {
  }
  
  if ($item->user_id > 0) {
    try {
      $user = User::getInstance($item->user_id);
      $user->setFlag(User::FLAG_ABUSE);
    }
    catch (Exception $e) { $log->error($e); }
  }
  
  $item->incrementMeta(Meta::FIELD_ABUSES);
  
  $req->track();
  
  $res->sendMessage(l('abuse.report.ack'));

}
catch (AuthException $e) { $res->sendAuthenticationForm(); }
catch (Exception $e) { $err->add($e); }
