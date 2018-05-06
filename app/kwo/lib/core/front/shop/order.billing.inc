<?php

try {

  $user = $req->getUser();

  $order = Order::load();

  $ctx->profile = array();
  if ($user->hasProfile()) {
    $ctx->profile = $user->getProfile()->getAttributes();
    $profile = P('profile.customer');
    $ctx->user_customer_view = $user->getProfile($profile)->asAddress($req->getLocale());
  }

  $req->track(true, 'shop');

  $ctx->user = $user;
  $ctx->snippet = Snippet::get('order.section.billing');
  $ctx->order = $order;
  $ctx->is_valid_customer = Order::isValidProfile($ctx->profile);

  $order->checkout();

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
