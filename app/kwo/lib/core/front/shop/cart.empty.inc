<?php

try {

  if ($req->getCookie()->getAttribute('order_id') < 1) {
    throw new Exception('missing order_id in cookie');
  }

  $order = Order::load();

  $order->markAsEvolved();

  if ($order->isClosed()) {
    throw new Exception('invalid order_id ('.$order->getId().')');
  }

  $order->clear();

  $order->calculate();

  $req->track(true, 'shop');

  $res->disableCache();

}
catch (Exception $e) { $log->error($e); }