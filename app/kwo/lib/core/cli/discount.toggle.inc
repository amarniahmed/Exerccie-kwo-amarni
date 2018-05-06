<?php

try {
  
/*  $filters = array('type' => Discount::TYPE_CATALOG,
                   'status' => Discount::STATUS_ONLINE);
  $opts = array('filters' => $filters);
  foreach (Discount::collect($opts) as $discount) {
    $discount->apply();
  }*/

  $rs = $dbo->query('SELECT id FROM '.T('discount')
                    .' WHERE status='.Discount::STATUS_PENDING
//                    .  ' AND type='.Discount::TYPE_CATALOG
                    .' AND "'.DH::today().'" BETWEEN available_from AND available_to');
  foreach ($rs as $row) {
    $discount = Discount::getInstance((int) $row['id']);
    $discount->setStatus(Discount::STATUS_ONLINE);
    $log->trace('discount ['.$discount->name.'] is now online');
  }
  
  $rs = $dbo->query('SELECT id FROM '.T('discount')
                    .' WHERE status='.Discount::STATUS_ONLINE
//                    .  ' AND type='.Discount::TYPE_CATALOG
                    .' AND "'.DH::DATE_NULL.'" <> available_to'
                    .' AND "'.DH::today().'" > available_to');
  foreach ($rs as $row) {
    $discount = Discount::getInstance((int) $row['id']);
    $discount->setStatus(Discount::STATUS_OFFLINE);
    $log->trace('discount ['.$discount->name.'] is now offline');
  }

}
catch (Exception $e) { $err->add($e); }
