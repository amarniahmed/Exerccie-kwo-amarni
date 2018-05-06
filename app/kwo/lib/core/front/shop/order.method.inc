<?php

try {

  $user = $req->getUser();

  $order = Order::load();

  $opts = array('filters' => array('|>|status' => 0),
                'order' => array('position' => 'asc'),
                'locale' => true);

  if ($order->discount_amount > 0 && $order->total_amount < 1) {
    $opts['filters']['type'] = Method::TYPE_GIFTCARD;
  }
  elseif ($order->total_amount < 1) {
    $opts['filters']['type'] = Method::TYPE_VOID;
  }
  else {
    $opts['filters']['|<>|type'] = Method::TYPE_GIFTCARD;
  }

  $methods = Method::collect($opts);

  $method_id = $order->method_id;
  if ($method_id < 1) {
    $method_id = $methods->first()->getId();
  }

  $req->track(true, 'shop');

  $ctx->trackers = (array) P('shop.trackers');
  $ctx->snippet = Snippet::get('order.section.method');
  $ctx->methods = $methods;
  $ctx->method_id = $method_id;
  $ctx->order = $order;
  $ctx->user = $user;

  $order->checkout();

  $res->useTemplate();

}
catch (Exception $e) {
  $log->error($e);
  $err->add($e);
}
