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
  $country_id = $order->getShippingCountryId();
  $country = strtolower(Country::code($country_id));
  foreach (Carrier::collect($opts) as $carrier) {
    $countries = strtolower($carrier->countries);
    if (!empty($countries)) { 
      $countries = SH::asSet($countries); 
      if (!in_array($country, $countries)) continue ;
    } 
    if ($carrier->threshold_min > 0 && 
        $order->total_amount < $carrier->threshold_min) {
      continue ;
    }
    if ($carrier->threshold_max > 0 && 
        $order->total_amount > $carrier->threshold_max) {
      continue ;
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
