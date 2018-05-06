<?php

try {

  $order = Order::load();

  if ($order->isClosed()) {
    $log->trace('invalid order_id ('.$order->id.')');
    $ctx->id = $order->getId();
    return ;
  }

  $attrs = array();
  $fields = array('addressee_id', 'method_id', 'carrier_id', 'shipping_method', 
                  'shipping_country_id', 'shipping_admin2');
  foreach ($fields as $field) {
    if (!$req->hasAttribute($field)) continue ;
    $attrs[$field] = $req->getAttribute($field);
    if ($field != 'shipping_admin2') {
      $attrs[$field] = (int) $attrs[$field];
    }
  }
  
  $attrs['checkedout_at'] = DH::now();
  $order->commit($attrs);
  
  if ($req->hasAttribute('addressee_id', true)) {
    try {
      $addressee = $order->getAddressee();
      $row = array('shipping_country_id' => $addressee->country_id,
                   'shipping_admin2' => $addressee->admin2);
      $order->setAttributes($row);
    }
    catch (Exception $e) {
      $log->error($e);
    }
  }
  elseif ($req->hasAttribute('addressee_id')) {
    $row = array('shipping_country_id' => $order->getBillingCountryId(),
                 'shipping_admin2' => $order->getBillingAdmin2());
    $order->setAttributes($row);
  }
  
  if ($order->method_id < 1) {
    $id = $dbo->fetchValue('SELECT id'
                           .' FROM '.T('method')
                           .' WHERE status=1'
                           .' ORDER BY position ASC');
    if ($id < 1) {
      throw new Exception(l('error.order.method'));
    }
    $order->setAttribute('method_id', $id);
  }

  if (is_array($req->billing)) {
    $req->getUser()->setProfile($req->billing, P('profile.customer'));
    if ($order->addressee_id < 1) {
      $row = array('shipping_country_id' => $req->billing['country_id'],
                   'shipping_admin2' => $req->billing['admin2']);
      $order->setAttributes($row);
    }
  }

  if (is_array($req->flags)) {
    if (isset($req->flags['credit'])) {
      // HUGUES FLAG_CREDIT does not exist
      $order->toggleFlag(Order::FLAG_CREDIT, $req->flags['credit']);
    }
    if (isset($req->flags['handling'])) {
      // HUGUES ajout set flag
      $order->toggleFlag(Order::FLAG_HANDLING, $req->flags['handling']);
    }
  }

  if ($req->step === 'finalize' || $req->step === 'billing') {
    $profile = $req->getUser()->getProfile(P('profile.customer'));
    if (!Order::isValidProfile($profile)) {
      throw new Exception(l('error.address.billing'));
    }
  }

  $order->calculate();

  $ctx->id = $order->getId();

}
catch (Exception $e) { $err->add($e); }
