<?php

try { 

  $req->cleanAttributes();

  $user = $req->getUser();

  $cookie = $req->getCookie();

  $order_id = (int) $cookie->getAttribute('order_id');
  if ($order_id < 1) {
    throw new Exception('invalid order');
  }

  $order = Order::load();

  $coupon = (string) $req->coupon;

  $discount_id = (int) $dbo->fetchValue('SELECT discount_id'
                                        .' FROM '.Discount::TABLE_CONDITION
                                        .' WHERE attribute="coupon"'
                                        .  ' AND value=:coupon:',
                                        array(':coupon:' => $coupon));
  if ($discount_id < 1) {
    throw new Exception(l('error.coupon.invalid'));
  }

  $discount = Discount::getInstance($discount_id);
  if ($discount->status != Discount::STATUS_ONLINE) {
    throw new Exception(l('error.coupon.outdated'));
  }

  if (!$discount->isAvailableForUser($user)) {
    throw new Exception(l('error.coupon.user'));
  }
  
  if ($discount->hasFlag(Discount::FLAG_SINGLE_USE) && 
      $discount->isUsedByUser($user, $order)) {
    throw new Exception(l('error.coupon.burnt'));
  }

  if ($discount->quota >= 1 && 
      $discount->getUseCount() > $discount->quota) {
    throw new Exception(l('error.coupon.quota'));
  }

  $order->setAttribute('coupon', $coupon);
  if (!$discount->match($order)) {
    throw new Exception($discount->getProperty('message'));
  }

  $ids = $dbo->asSet('SELECT discount_id'
                     .' FROM '.Order::TABLE_DISCOUNT
                     .' WHERE order_id='.$order->getId()
                     .  ' AND discount_id<>'.$discount->getId());
  foreach ($ids as $id) {
    try {
      $d = Discount::getInstance($id);
      if ($d->isBlocking()) {
        throw new Exception(l('error.coupon.blocking'));
      }
    }
    catch (Exception $e) {
      $log->error($e);
    }
  }

  $req->track(true, 'shop');
  
  $cookie->setAttribute('coupon', $coupon);

  $ctx->coupon = $coupon;
  $res->sendMessage(l('coupon.accepted'));
  
}
catch (AuthException $e) { $res->sendAuthenticationForm(); }
catch (Exception $e) { 
  $req->track('coupon '.$coupon, Tracker::CATEGORY_ERROR);
  if ($order instanceof Order) {
    $order->setAttribute('coupon', '');
  }
  $err->add($e); 
}