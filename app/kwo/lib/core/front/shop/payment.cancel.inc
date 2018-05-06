<?php

try {

  $order = Order::getInstanceByKey($req->item);

  if ($order->status == Order::STATUS_PENDING) {
    $order->setAttribute('status', Order::STATUS_NONE);
    $cookie = $req->getCookie();
    $cookie->setAttribute('order_id', $order->getId());
  }

  $res->sendRedirect('/cart');

}
catch (Exception $e) { $err->add($e); }

