<?php

$parameter['id'] = 507;
$parameter['name'] = 'order';
$parameter['label'] = 'commande';
$parameter['has_many'] = array('purchase' => array('dependent' => true),
                               'shipment' => array('dependent' => true),
                               'payment' => array('dependent' => true),
                               'discount' => array('through' => 'core_discount_order'));