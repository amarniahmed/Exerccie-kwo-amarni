<?php

try { 

  $id = $req->id ? (int) $req->id : true;
  $addressee = Addressee::getInstance($id);

  $elts = array(array('elt' => 'text',
                      'name' => 'record[first_name]',
                      'value' => $addressee->first_name,
                      'label' => l('first_name'),
                      'required' => true),
                array('elt' => 'text',
                      'name' => 'record[last_name]',
                      'value' => $addressee->last_name,
                      'label' => l('last_name'),
                      'required' => true),
                array('elt' => 'text',
                      'name' => 'record[address]', 
                      'value' => $addressee->address, 
                      'label' => l('address'),
                      'required' => true),
                array('elt' => 'text',
                      'name' => 'record[address_extra]', 
                      'value' => $addressee->address_extra, 
                      'label' => l('address.extra')),
                array('elt' => 'text',
                      'name' => 'record[city]', 
                      'value' => $addressee->city, 
                      'label' => l('city'),
                      'required' => true),
                array('elt' => 'text',
                      'name' => 'record[region]', 
                      'value' => $addressee->region, 
                      'label' => l('region')),
                array('elt' => 'text',
                      'name' => 'record[postal_code]', 
                      'value' => $addressee->postal_code, 
                      'label' => l('postal_code'),
                      'required' => true),
                array('elt' => 'select',
                      'name' => 'record[country_id]', 
                      'value' => $addressee->country_id, 
                      'label' => l('country'),
                      'required' => true),
                array('elt' => 'text',
                      'name' => 'record[phone]', 
                      'value' => $addressee->phone, 
                      'label' => l('phone')));

  $events = array('submit' => array('label' => l('save')));

  $opts = array('layout' => 'inline',
                'wide' => true,
                'elts' => $elts,
                'events' => $events);

  $ctx->form = Elt::form($opts);

  $req->track(true, 'shop');

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
