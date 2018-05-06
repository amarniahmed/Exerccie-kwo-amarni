<?php

try { 

  $card = array('numero' => substr(preg_replace('/([^0-9])/', '', $req->card['numero']), 0, 16),
                'annee' => (int)$req->card['annee'],
                'mois' => (int)$req->card['mois']);
  $cvx_length = $req->card['name'] != 'amex' ? 3 : 4;
  $card['cvx'] = substr(preg_replace('/([^0-9])/', '', $req->card['cvx']), 0, $cvx_length);

  if (!$req->hasAttribute('cgv', true)) {
    throw new Exception('Vous devez accepter les CGV pour continuer.');
  }

  $user = $req->getUser();

  $order = Order::load();

  if (!$user->hasAccessTo($order)) {
    throw new MaliciousException();
  }

  if ($order->isPaid()) {
    $res->sendRedirect('/payment.response/-/id/'.$order->getId());
    return;
  }

  $order->calculate();
  $order->setVisit($req);

  $req->track(true, 'shop');

  $payment = Payment::getInstance($order->payment_id);

  if (!$payment->call($order, $card)) {
    throw new Exception($order->payment_comment);
  }

  $req->getCookie()->removeAttribute('order_id');
  
  $ctx->order_id = $order->getId();

}
catch (AuthException $e) { $err->add(l('error.access')); }
catch (Exception $e) { 
  $log->error($e, __METHOD__);
  $err->add($e); 
}