<?php

try {

  $item = Item::getInstance(MODEL, (int) $req->id);

  $geo = array();
  $geolocation_id = Geolocation::id($item);
  if ($geolocation_id > 1) {
    $geo = Geolocation::getInstance($geolocation_id)->getAttributes();
  }

  $ctx->item = $item;
  $ctx->geo = $geo;
  $ctx->formatted_address = Geolocation::asAddress($item);

  $res->useTemplate('core:item.geocode');

}
catch (Exception $e) { $err->add($e); }
