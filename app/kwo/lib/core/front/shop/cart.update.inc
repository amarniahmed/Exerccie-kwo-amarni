<?php

try { 

  $order_id = (int) $req->getCookie()->getAttribute('order_id');
  if ($order_id < 1) {
    return ;
  }

  $order = Order::load();
  
  $order->markAsEvolved();

  $attrs = array();
  $country_id = (int) $req->getAttribute('shipping_country_id');
  if ($country_id != $order->getAttribute('shipping_country_id')) {
    $attrs['shipping_country_id'] = $country_id;
  }
  $admin2 = (int) $req->getAttribute('shipping_admin2');
  if ($admin2 != $order->getAttribute('shipping_admin2')) {
    $attrs['shipping_admin2'] = $admin2;
  }
  if (!empty($attrs)) {
    $order->commit($attrs);
  }

  $deletion_count = 0;
  $purchases = array();

  foreach ($req->purchases as $id => $qty) {
    if ($qty < 1) {
      try {
        $req->track('cart.purchase.remove', 'shop');
        $purchase = Purchase::getInstance($id);
        $purchase->destroy();
        $deletion_count++;
      }
      catch (Exception $e) { $log->debug($e); }
      continue ;
    }
    $purchases[$id] = $qty;
  }

  try {
    $order->update($purchases);
  }
  catch (Exception $e) { $err->add(join("\n", $e->errors)); }

  $order->calculate();

  $purchase_count = $order->getPurchaseCount();

  if ($purchase_count < 1 && !DH::isNull($order->expire_at)) {
    $order->commit(array('expire_at' => DH::DATE_NULL));
  }

  $ctx->purchase_count = $purchase_count;
  
  $req->track(true, 'shop');

  if ($deletion_count >= 1) {
    $req->track('purchase.deletion', 'shop');
  }

  $res->disableCache();

}
catch (Exception $e) { $log->error($e); }
