<?php

try { 

  $user = $req->getUser();

  $order = Order::getInstance((int) $req->id);
  if (!$user->hasAccessTo($order)) {
    return new MaliciousException();
  }

  //  $order->setFlag(Order::FLAG_RETURN);

  $comment = SH::clean($req->comment, array('strict' => true));

  $body = array('commande n°' => $order->getId(),
                'client' => nl2br($order->billingAddress()),
                'livraison' => nl2br($order->shippingAddress()),
                'commentaire' => $comment);

  if ($req->node_id >= 1) {
    $node = Node::getInstance((int) $req->node_id);
    $body['motif'] = nl2br(join("\n", $node->getBreadcrumb()));
  } 

  $recipient = P('contact.admin');

  $mail = Mail::getInstance();
  $mail->addRecipient($recipient);
  $mail->setSubject('demande de retour');
  $mail->setBody($body, true);
  $mail->setFrom($user);
  $mail->send();

  $res->sendMessage(Snippet::content('order.return.confirm'));

}
catch (Exception $e) { $err->add($e); }
