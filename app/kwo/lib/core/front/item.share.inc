<?php

try {

  $item = Item::getInstanceByKey($req->item);

  $url = null;
  if (method_exists($item, 'asShareHash')) {
    // url, title, description, image
    $provider = $req->provider;
    $h = $item->asShareHash();
    $url = SocialClient::getInstance()->getShareUrl($h, $provider);
    if ($url) {
      $item->track(Interaction::TYPE_SHARE, SocialClient::provider2id($provider));
    }
  }

  $ctx->url = $url;

}
catch (Exception $e) { $err->add($e); }
