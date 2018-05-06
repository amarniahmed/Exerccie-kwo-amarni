<?php

try { 
$req->debug();
  $user = $req->getUser();
  
  $order = Order::load();

  if ($order->isPaid()) {
    throw new Exception('order ('.$order->getId().') already paid');
  }

  if (!$user->hasAccessTo($order)) {
    $log->trace('order access failure : order('.$order->getId().') user('.$user->id.') ['.$user->email.']');
    throw new MaliciousException();
  }

  $order->calculate();
  $order->setStatus(Order::STATUS_PENDING);
  $order->setVisit($req);
  $order->setUser($user);
  $order->setLocale($user);

  $req->track(true, 'shop');

  $req->getCookie()->removeAttribute('order_id');

  $method = Method::getInstance($order->method_id);

  $out = $method->request($order);

  $log->trace($out);

  $res->write($out);

}
catch (AuthException $e) { 
  $err->add(l('error.access')); 
}
catch (Exception $e) { 
  if ($order) {
    $log->trace('order [id='.$order->id.'] [status='.$order->status.']');
  }
  $log->error($e, __METHOD__);
  $err->add($e); 
}