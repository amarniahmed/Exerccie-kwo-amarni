<?php

try {

  //  Payment::response();

  $user = $req->getUser();

  $order = Order::getInstanceByKey($req->item);

  if (!$user->hasAccessTo($order)) {
    $log->error('order ('.intval($order->id).') bad user_id ('.$req->getUserId().')');
    throw new MaliciousException();
  }

  if ($order->status <= Order::STATUS_ERROR || $order->status > Order::STATUS_PAID) {
    $log->error('order ('.intval($order->id).') bad status ('.$order->status.')');
    throw new Exception('Etat de commande invalide.');
  }

  $method = $order->getMethod();

  $snippet_code = 'payment.response.'.$method->getType();
  if (!empty($method->snippet_response)) {
    $snippet_code = $method->snippet_response;
  }

  $purchases = $order->getPurchases();

  $ctx->trackers = (array) P('shop.trackers');
  $ctx->snippet_code = $snippet_code;
  $ctx->order = $order;
  $ctx->purchases = $purchases;
  $ctx->user = $user;
//  $ctx->method_type = $method->getType();
  //  $ctx->psp = $psp;

  $res->setHead('paiement');
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }