<?php

try {

  $item = Item::getInstance(MODEL, $req->id);

  $ctx->setAttributes($req);
  $ctx->recipients = SH::asSet($req->recipients);
  $ctx->item = $item;

  $item->onLetterSend();

  if (count($ctx->recipients) < 1) {
    throw new Exception(c(l('veuillez préciser les adresses emails des destinataires')));
  }

  foreach ($ctx->recipients as $email) {
    $mail = Mail::getInstance();
    $mail->setSubject($ctx->subject);
    $mail->setBody($ctx->body, true);
    $mail->setRecipient($email);
    if (!empty($ctx->from_email)) {
      $mail->setFrom($ctx->from_email, $ctx->from_name);
    }
    $mail->send();
  }

  $res->sendMessage(c(l('le message a bien été envoyé à ((count)) destinataire(s)', array('count' => count($ctx->recipients)))));

}
catch (Exception $e) { $err->add($e); }