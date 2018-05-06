<?php

try {

/*  $opts = array('filters' => array('|<=|available_from' => DH::now(),
                                   '|>=|available_to' => DH::now(),
                                   '|&|flags' => Coupon::FLAG_PUBLIC),
                'order' => array('available_to' => 'desc'));
  $coupons = Coupon::collect($opts); 

  $ctx->coupons = $coupons;*/

  $discounts = Discount::collectByUser($user);

  $ctx->discounts = $discounts;

  $res->setMenu('discounts');
  $res->setTitle(l('account.discounts'));

}
catch (Exception $e) { $err->add($e); }
