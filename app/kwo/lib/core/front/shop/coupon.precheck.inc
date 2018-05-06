<?php

try {

  $req->cleanAttributes();

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
  if (!$discount->isOnline()) {
    throw new Exception(l('error.coupon.outdated'));
  } 

  $cookie = $req->getCookie();
  $cookie->setAttribute('coupon', $coupon);
  
  $res->sendMessage($discount->getProperty('message'));

}
catch (Exception $e) { $err->add($e); }
