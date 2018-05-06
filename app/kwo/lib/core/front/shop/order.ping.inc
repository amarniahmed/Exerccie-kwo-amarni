<?php

try {

  $ctx->order = null;

  if ($req->getCookie()->getAttribute('order_id') < 1) return ;

  $order = Order::load();

  if ($order->isClosed()) {
    return ;
  }

  if ($order->hasExpired()) {
    $order->clear();
    $req->track('cart expiration', 'shop');
  }

  $ctx->order = $order;

}
catch (Exception $e) { $err->add($e); }
