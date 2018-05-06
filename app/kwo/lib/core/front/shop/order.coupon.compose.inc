<?php

try {
  
  $cookie = $req->getCookie();
  
  $order_id = (int) $cookie->getAttribute('order_id');
  if ($order_id < 1) {
    throw new Exception('invalid order');
  }
  
  $user = $req->getUser();

  $order = Order::load();

  $coupon = '';
  $display_link_check = false;
  $display_link_remove = false;

  if (!empty($order->coupon)) {
    $display_link_remove = true;
    $coupon = $order->coupon;
  }
  elseif ($cookie->hasAttribute('coupon', true)) {
    $display_link_check = true;
    $display_link_remove = true;
    $coupon = $cookie->getAttribute('coupon');
  }
  else {
    $display_link_check = true;
  }

  $ctx->order = $order;
  $ctx->coupon = $coupon;
  $ctx->display_link_check = $display_link_check;
  $ctx->display_link_remove = $display_link_remove;

  $order->checkout();

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
