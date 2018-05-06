<?php

try {

  $cookie = $req->getCookie();

  $order_id = (int) $cookie->getAttribute('order_id');
  if ($order_id < 1) {
    throw new Exception(l('error.order.invalid'));
  }

  $user = $req->getUser();

  $order = Order::load();
  
  if ($cookie->hasAttribute('coupon')) {
    $cookie->removeAttribute('coupon');
  }

  if (!empty($order->coupon)) {
    $order->setAttribute('coupon', '');
  }
  
  $req->track(true, 'shop');

  $ctx->coupon = '';

}
catch (Exception $e) { $err->add($e); }
