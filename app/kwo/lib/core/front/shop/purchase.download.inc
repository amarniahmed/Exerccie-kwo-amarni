<?php

try {

  if (!$req->isAuthenticated()) {
    $res->sendRedirect('/account/order.list');
  }

  $user = $req->getUser();

  $purchase = Item::getInstanceByKey($req->item);
  if (!$purchase->isA('purchase') || !$purchase->isDigital()) {
    throw new Exception('cet achat ne peut être téléchargé');
  }

  $order = $purchase->getOrder();

  $count = $dbo->fetchValue('SELECT COUNT(*)'
                            .' FROM '.Interaction::TABLE
                            .' WHERE model_id='.$purchase->getModelId()
                            .  ' AND design_id='.$purchase->getId()
                            .  ' AND type='.Interaction::TYPE_DOWNLOAD
                            .  ' AND DATEDIFF("'.DH::today().'", created_on) < 2');
    
  if ($count >= 6) {
    $mail = Mail::getInstance();
    $mail->setSubject('download max');
    $mail->setBody($purchase);
    $mail->addRecipient(P('contact.admin'));
    $mail->send();
  }

  if (!$user->hasAccessTo($order)) {
    return new MaliciousException();
  }

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
