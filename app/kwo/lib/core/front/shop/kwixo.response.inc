<?php

try {

  //  Payment::response();
  require LIB_PATH.'/core/class/psp/Kwixo.class.inc';
  
  $user = $req->getUser();

  if ($req->hasAttribute('item')) {
    $order = Order::getInstanceByKey($req->item);
  }
  else {
    $order = Order::getInstance((int) $req->id);
  }

  if (!$user->hasAccessTo($order)) {
    $log->error('order ('.intval($req->id).') bad user_id ('.$req->getUserId().')');
    throw new MaliciousException();
  }
  
  list ($order_id, $payment_id, ) = explode('x', $req->RefID);


  
  if ($order->getId() != $order_id) {
    $log->error('order ('.intval($req->id).') bad RefID ('.$req->RefID.')');
    throw new MaliciousException();
  }

  if ($order->status <= Order::STATUS_ERROR || $order->status > Order::STATUS_PAID) {
    $log->error('order ('.intval($req->id).') bad status ('.$order->status.')');
    throw new Exception('Etat de commande invalide.');
  }
  
  $amount = number_format($order->total_amount, 2, '.', '');
  
  if ($req->Tag == 1 && (int) ($amount * 100) != (int) $req->Montant) {
    $log->error('order ('.intval($req->id).') bad amount ('.$req->Montant.')');
    throw new MaliciousException();
  }
  
  $all_parameters = P('extension.shop.ips.parameters');
  $parameters = $all_parameters['kwixo'];
  
  if ($req->Tag == 1) {
  if (P('app.kernel') == '64') {
    $MD5 = new HashMD564();
  } else {
    $MD5 = new HashMD532();
  }
    
  $HashControl = $MD5->hash($parameters['privatekey'].$req->RefID.$req->TransactionID);
    
  if ($HashControl != $req->HashControl) {
    $log->error('order ('.intval($req->id).') invalid HashControl');
    throw new MaliciousException();
  }
  }

  $method = $order->getMethod();

 
  //  $ctx->psp = $psp;

  $snippet_code = 'payment.response.'.$method->getType();
  if (!empty($method->snippet_response)) {
    $snippet_code = $method->snippet_response;
  }
  
  $purchases = $order->getPurchases();

  $ctx->snippet_code = $snippet_code;
  $ctx->order = $order;
  $ctx->purchases = $purchases;
  $ctx->user = $user;

  $res->setHead('paiement');
  
  if ($req->Tag == 1) {
    $res->useTemplate('payment.response');
  } else {
    $res->useTemplate('payment.cancel');
  }

}
catch (Exception $e) { $err->add($e); }