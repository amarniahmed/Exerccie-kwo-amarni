<?php

try {

  $user = $req->getUser();

  $order = Order::load();

//  $coupon = $order->coupon_id > 0 ? $order->getCoupon() : null;

  $req->track(true, 'shop');

  $ctx->snippet = Snippet::get('order.section.discount');
  $ctx->order = $order;
  $ctx->coupon = $order->coupon;
  $ctx->coupon_unsure = '';
  $ctx->coupon_link = l('select');

  if ($req->getCookie()->hasAttribute('coupon')) {
    $ctx->coupon_unsure = $req->getCookie()->getAttribute('coupon');
    $ctx->coupon_link = 'tester le coupon "'.$ctx->coupon_unsure.'"';
  }
//  $ctx->coupon_id = $coupon ? $coupon->getId() : 0;
  $ctx->user = $user;

  $order->checkout();

  $res->useTemplate();

}
catch (Exception $e) {
  $log->error($e);
  $err->add($e);
}
