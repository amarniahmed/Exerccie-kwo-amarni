<?php

try {

  if (!$req->hasAttribute('item')) {
    return $res->sendRedirect('/');
  }

  $affiliate = Affiliate::getInstanceByKey($req->item);

  $source = Item::asSource($affiliate);

  $months = array();

  $rs = $dbo->query('SELECT DATE_FORMAT(created_on, "%Y-%m") AS `mk`,'
                    .     ' COUNT(*) AS `count`'
                    .' FROM '.T('visit')
                    .' WHERE source='.$source
                    .' GROUP BY `mk`'
                    .' ORDER BY `mk` DESC');

  foreach ($rs as $row) {
    $months[$row['mk']] = array('visits' => $row['count']);
  }

  $rs = $dbo->query('SELECT DATE_FORMAT(converted_at, "%Y-%m") AS `mk`,'
                    .     ' COUNT(*) AS `count`,'
                    .     ' SUM(worth) AS `turnover`'
                    .' FROM '.T('conversion')
                    .' WHERE source='.$source
                    .' GROUP BY `mk`'
                    .' ORDER BY `mk` DESC');

  foreach ($rs as $row) {
    $months[$row['mk']] = (array) $months[$row['mk']];
    $months[$row['mk']]['orders'] = $row['count'];
    $months[$row['mk']]['turnover'] = $row['turnover'];
  }
  
  $ctx->affiliate = $affiliate;
  $ctx->months = $months;

  $res->useTemplates(null, true);

}
catch (Exception $e) { $err->add($e); }
