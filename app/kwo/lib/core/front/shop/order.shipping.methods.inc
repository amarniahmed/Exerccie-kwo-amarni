<?php

try {

  $user = $req->getUser();
  
  $order = Order::load();

  $filters = array('app' => array(S('app')->getName(), ''),
                   'status' => Carrier::STATUS_ONLINE);
  $opts = array('filters' => $filters,
                'properties' => true,
                'order' => array('position' => 'asc'));
  $carriers = array();
  foreach (Carrier::collect($opts) as $carrier) {
    if (!empty($carrier->countries)) { 
      $country_id = $order->getShippingCountryId();
      $country = strtolower(Country::code($country_id));
      $countries = SH::asSet(strtolower($carrier->countries)); 
      if (!in_array($country, $countries)) continue ;
    } 
    $carriers[$carrier->getId()] = $carrier;
  }

  if (!empty($carriers) && !$carriers[$order->carrier_id]) {
    $carrier = reset($carriers);
    $order->setAttribute('carrier_id', $carrier->getId());
    $order->calculate();
  }

  $ctx->carriers = $carriers;
  $ctx->order = $order;
  $ctx->user = $user;

  $order->checkout();

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
