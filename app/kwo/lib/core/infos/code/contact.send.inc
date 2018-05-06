<?php

try { $req->debug();

  $contact = (array) $req->contact;

  if (strlen($contact['last_name']) < 2 || 
      strlen($contact['first_name']) < 2 || 
      !Email::isValid($contact['email'], true)) {
    throw new Exception('veuillez vérifier les 3 champs obligatoires');
  }
  
  foreach ($contact as $key => &$value) {
    $value = SH::clean($value, array('strict' => true));
  }

  $recipient = P('contact.admin');
  $subject = 'formulaire de contact';
  if ($req->faq_id >= 1) {
    $faq = Faq::getInstance($req->faq_id);
    $recipient = empty($faq->email) ? $recipient : $faq->email;
    $recipient = empty($faq->subject) ? $subject : $faq->subject;
  }

  $mail = Mail::getInstance('mail');
  $mail->addRecipient($recipient);
  $mail->setSubject($subject);
  $mail->setBody($contact, true);
  $mail->setFrom($contact['email'], 
                 $contact['first_name'].' '.$contact['last_name']);
  $mail->send();

  $res->sendMessage('Merci, votre message a bien été transmis à nos équipes.');

}
catch (Exception $e) { $err->add($e); }

