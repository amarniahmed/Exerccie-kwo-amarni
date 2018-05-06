<?php

try {

  $user = $req->getUser();

  $order = Order::load();
  
  $order->calculate();

  $purchases = $order->getPurchases();
  if ($purchases->count() < 1) {
    $log->error('no purchase');
    $res->sendRedirect('/cart');
  }


  $ctx->index = 1;
  $ctx->order = $order;
  $ctx->purchases = $purchases;

  $req->track(true, 'shop');

  $res->setHead();
  $res->useTemplate();

}
catch (AuthException $e) {
  $res->sendRedirect('/cart');
}
catch (Exception $e) {
  $req->track('order', Tracker::CATEGORY_ERROR);
  $log->trace($e);
  $err->add(l('error.cart.invalid'));
}