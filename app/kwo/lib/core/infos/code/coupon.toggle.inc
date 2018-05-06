<?php

try {

  $rs = $dbo->query('SELECT id FROM '.T('coupon')
                    .' WHERE status='.Coupon::STATUS_PENDING
                    .  ' AND type='.Coupon::TYPE_CATALOG
                    .  ' AND "'.DH::today().'" BETWEEN available_from AND available_to');
  foreach ($rs as $row) {
    $coupon = Coupon::getInstance((int) $row['id']);
    $coupon->setStatus(Coupon::STATUS_ONLINE);
    $log->trace('coupon ['.$coupon->name.'] is now online');
  }
  
  $rs = $dbo->query('SELECT id FROM '.T('coupon')
                    .' WHERE status='.Coupon::STATUS_ONLINE
                    .  ' AND type='.Coupon::TYPE_CATALOG
                    .  ' AND "'.DH::today().'" > available_to');
  foreach ($rs as $row) {
    $coupon = Coupon::getInstance((int) $row['id']);
    $coupon->setStatus(Coupon::STATUS_OFFLINE);
    $log->trace('coupon ['.$coupon->name.'] is now offline');
  }

}
catch (Exception $e) { $err->add($e); }

