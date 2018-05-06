<?php

try {

  $limit = '';
  if ($req->limit == 1) {
    $limit = ' LIMIT '.intval($req->limit_value);
  }

  $where = 'o.status>='.Order::STATUS_PAID;

  if (strlen($req->from) > 5) {
    $where .= ' AND o.paid_at >= "'.DH::toDb($req->from).'"';
  }

  if (strlen($req->to) > 5) {
    $where .= ' AND o.paid_at <= "'.DH::toDb($req->to).'"';
  }

  $methods = array();
  foreach (Method::collect() as $method) {
    $methods[$method->id] = Method::$types[$method->type];
  }

  $rs = $dbo->query('SELECT o.id, o.invoice_id, o.discount_amount, o.shipping_amount, o.purchase_amount,'
                    .     ' o.tax_amount, o.total_amount, o.paid_at, o.user_id, SUM(p.quantity) AS q'
                    .' FROM '.T('order').' AS o,'
                    .     ' '.T('purchase').' AS p'
                    .' WHERE '.$where.' AND p.order_id=o.id'
                    .' GROUP BY o.id'
                    .' ORDER BY o.id DESC'
                    .$limit);

  $columns = array('order_id', 'invoice_id', 'discount_amount', 'shipping_amount', 'purchase_amount',
                   'tax_amount', 'total_amount', 'paid_at', 'user_id',
                   'quantity', 'client', 'address', 'email', 'locale', 'payment_method');

  $opts = array('columns' => $columns,
                'encoding' => 'UTF-8',
                'filename' => 'commandes-'.DH::today());
  $csv = CsvWriter::getInstance($opts);

  if ($rs === false || $rs->numRows() < 1) return ;

  $opts = array('filters' => array('order_id' => 0));
  while ($row = $rs->getNext()) {
    $line = $row;
    $user = $address = '';
    $user = $user_name = null;
    try {
      $user = User::getInstance((int) $row['user_id']);
      $profile = $user->getProfile();
      $user_name = $profile->first_name.' '.$profile->last_name;
      $address = join(' ', array($profile->address,
                                 $profile->address_extra,
                                 $profile->postal_code,
                                 $profile->city,
                                 Country::code($profile->country_id)));
    }
    catch (Exception $e) {
      $user = '** user ('.$row['user_id'].') **';
    }
    $line[] = $user_name;
    $line[] = $address;
    $line[] = $user instanceof ActiveRecord ? $user->email : '';
    $line[] = $user instanceof ActiveRecord ? Locale::toCode($user->locale) : '';//$line[] = $user instanceof ActiveRecord ? I18N::locale2code($user->locale) : '';
    $set = array();
    $opts['filters']['order_id'] = $row['id'];
    foreach (Payment::collect($opts) as $payment) {
      $set[] = $methods[$payment->method_id];
    }
    sort($set);
    $line[] = join(',', $set);
    $csv->append($line);
  }

}
catch (Exception $e) { $err->add($e); }
