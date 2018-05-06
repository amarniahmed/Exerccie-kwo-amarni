<?php

try {

  $order = Order::getInstance((int) $req->id);
  $user = $order->getUser();

  $locale = $user->getLocale();

  $ctx->order = $order;
  $ctx->taxes = $order->getTaxes();
  $ctx->locale = $locale; 
  
  $order->checkout();

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }