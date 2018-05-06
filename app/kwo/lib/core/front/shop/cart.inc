<?php

try {

  try {
    $order = Order::load();
  }
  catch (Exception $e) {
    $res->useTemplate();
    return ;
  }

  $order->calculate();

  $opts = array('order' => array('id' => 'asc'));

  $purchases = $order->getPurchases($opts);

  foreach ($purchases as $purchase) {
    $ctx->product_count += $purchase->quantity;
    $ctx->total += $purchase->getPrice() * $purchase->quantity;
  }

  $ctx->shipping_amount = $order->shipping_amount * (1+Order::SHIPPING_TAX / 100);
  $ctx->total_amount = $order->total_amount;
  $ctx->coupon = $req->getCookie()->getAttribute('coupon');
  $ctx->order = $order;
  $ctx->countries = I18N::getCountries($res->getLocale());
  $ctx->purchases = $purchases;

  $order->checkout();

  $req->track(true, 'shop');

  $res->disableCache();
  $res->setHead();
  $res->useTemplate();

}
catch (Exception $e) {
  $req->track('cart', Tracker::CATEGORY_ERROR);
  $log->error($e);
  $err->add(l('error.cart.invalid'));
}
