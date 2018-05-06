<?php

try {

  $order = Order::load(true);

  $item = Item::getInstanceByKey($req->item);

  $variations = is_array($req->variations)
              ? serialize($req->variations)
              : $req->variations;

  $opts = array('quantity' => $req->quantity,
                'variations' => $variations);
  $purchase = $order->addPurchase($item, $opts);

  $order->markAsEvolved();

  $item->track(Interaction::TYPE_PURCHASE,
               array('value' => $order->getId()));



  $ctx->variations = $req->variations;
  $ctx->item = $req->item;

  $req->track(true, 'shop');
  
  $res->disableCache();

}
catch (Exception $e) { $err->add($e); }
