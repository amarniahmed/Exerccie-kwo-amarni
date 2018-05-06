<?php

try { 

  $opts = array('include' => array('user'));
  $order = Item::getInstanceByKey($req->item, $opts);

  $merchant = Merchant::getInstance($order->merchant_id);

  $user = $order->getUser();

  $ctx->merchant = $merchant;
  $ctx->order = $order;
  $ctx->locale = $user->locale;

  $res->setLocale($user->locale);
  $res->useTemplates(null, true);
  
}
catch (Exception $e) { $err->add($e); }
