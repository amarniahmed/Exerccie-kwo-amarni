<?php

try { 

  $user = $req->getUser();

  $order = Order::load();

  $profile = $user->getProfile(P('profile.customer'));

  if (!Order::isValidProfile($profile)) {
    throw new Exception(l('error.address.billing'));
  }
  
  $order->setVisit($req);
  $order->setStatus(Order::STATUS_PENDING);

  try {
    foreach ($this->getPurchases() as $purchase) {
      if ($purchase->quantity < 1) continue ;
      $item = $purchase->getItem();
      $item->track(Interaction::TYPE_PURCHASE,
                   array('value' => $order->getId()));
    }
  }
  catch (Exception $e) { $log->error($e); }
  
  $cookie = $req->getCookie();
  $cookie->removeAttribute('order_id');
  $cookie->removeAttribute('coupon');
  
  $ctx->id = $order->getId();
  
}
catch (AuthException $e) { $res->sendRedirect('/cart'); }
catch (Exception $e) { $err->add($e); }
