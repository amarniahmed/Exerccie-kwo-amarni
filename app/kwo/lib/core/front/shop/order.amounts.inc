<?php

try { 

  usleep(300000);

//  $user = $req->getUser();
  $order = Order::load();

  $discounts = array();
  if ($order->discount_amount > 0) {
    $rs = $dbo->query('SELECT * FROM '.Order::TABLE_DISCOUNT
                      .      ' WHERE order_id='.$order->getId()); 
    foreach ($rs as $row) {
      try {
        $discount = Discount::getInstance((int) $row['discount_id']);
        $discounts[] = array('id' => $discount->getId(),
                             'label' => $discount->getProperty('label', $locale),
                             'amount' => $row['amount']);
      }
      catch (Exception $e) { $log->error($e); }
    } 
  }

  $ctx->discounts = $discounts;
  $ctx->order = $order;
//  $ctx->user = $user;

  $order->checkout();

  $res->useTemplate();  

}
catch (Exception $e) { 
  $log->error($e);
  $err->add($e); 
}
