<?php

try {

  $opts = array('filters' => array('|>=|status' => Order::STATUS_PENDING,
                                   T('order').'.user_id' => $user->getId()),
                'reverse' => true,
                'metas' => array('created_at'));
  
  $ctx->orders = Order::collect($opts);

  $res->setMenu('orders');
  $res->setTitle(l('account.orders'));

}
catch (Exception $e) { $err->add($e); }