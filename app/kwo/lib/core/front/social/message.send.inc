<?php

try { 

  $req->cleanAttributes();

  $user = $req->getUser();

  $name = $req->name;
  $content = $req->content;
  
  if (empty($name) || empty($content)) {
    throw new Exception(l('error.fields.required.all'));
  }

  if (strlen($name) > 90) {
    throw new Exception(l('error.title.size'));
  }

  if (strlen($content) > 20000) {
    throw new Exception(l('error.content.size'));
  }

  $recipient = Item::getInstanceByKey($req->item);
  if (true) {
    Message::send($user, array($recipient), $name, $content);
  }
  else {
    $content .= "\n\n".c(l('sent.by')).' '.$user->name;
    $mail = Mail::getInstance();
    $mail->setFrom($user);
    $mail->addRecipient($recipient);
    $mail->setSubject($name);
    $mail->setBody(nl2br($content));
    $mail->send();
  }

  $req->track('message.send');
  
  $res->sendMessage(c(l('message.sent')));

}
catch (AuthException $e) { $err->add($e); }
catch (Exception $e) { $err->add($e); }

