<?php

try { 

  $user = $req->getUser();
  
  $order = Order::load();

  $filters = array('user_id' => $user->getId(),
                   '|>|status' => 0);
  $opts = array('filters' => $filters);
  $addressees = Addressee::collect($opts);

  $req->track(true, 'shop');

  $ctx->snippet = Snippet::get('order.section.shipping');
  $ctx->addressees = $addressees;
  $ctx->order = $order;
  $ctx->user = $user;

  $order->checkout();

  $res->useTemplate();

}
catch (Exception $e) {
  $log->error($e);
  $err->add($e); 
}
