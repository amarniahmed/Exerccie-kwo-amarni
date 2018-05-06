<?php

try {
 
  $limit = 20;

  $ctx->stats = null;

  $res->useTemplate();

  $wheres = $wherep = '1';

  $format = '';
  $value = '';
  if ($req->year > 0) {
    $format .= '%Y';
    $value .= $req->year;
  }
  if ($req->month > 0) {
    $format .= '%c';
    $value .= $req->month;
  }
  if ($req->day > 0) {
    $format .= '%e';
    $value .= $req->day;
  }
  if (!empty($format)) {
    $wheres = ' DATE_FORMAT(o.`started_at`, "'.$format.'")="'.$value.'"';
    $wherep = ' DATE_FORMAT(o.`paid_at`, "'.$format.'")="'.$value.'" AND o.status>='.Order::STATUS_PAID;
  }
  
  $n = $dbo->fetchValue('SELECT SUM(total_amount) FROM '.T('order').' AS o WHERE '.$wherep);
  $h['chiffre d’affaires TTC'] = (int) $n;
  
  $n = $dbo->fetchValue('SELECT count(*) FROM '.T('order').' AS o WHERE '.$wherep);
  $h['nbre de commandes payées'] = (int) $n;
  
  $n = $dbo->fetchValue('SELECT count(*) FROM '.T('order').' AS o WHERE '.$wheres); 
  $h['nbre de paniers'] = (int) $n;
  
  $n = $dbo->fetchValue('SELECT COUNT(user_id) FROM '.T('order').' AS o WHERE '.$wherep);
  $h['nbre de clients'] = (int) $n;
  
  $n = $dbo->fetchValue('SELECT count(DISTINCT user_id) FROM '.T('order').' AS o WHERE '.$wherep);
  $h['nbre de clients différents'] = (int) $n;

  $n = $dbo->fetchValue('SELECT SUM(discount_amount) FROM '.T('order').' AS o WHERE '.$wherep);
  $h['montant des réductions'] = (int) $n;

  $rs = $dbo->query('SELECT DISTINCT(concat(model_id, "-", record_id)) FROM shop_purchase AS p, '.T('order').' AS o WHERE p.order_id=o.id AND '.$wherep);
  $h['nbre d’articles différents vendus'] = (int) $rs->numRows();
  
  $rs = $dbo->query('SELECT SUM(quantity) AS s1, SUM(quantity*price) AS s2 FROM shop_purchase AS p, '.T('order').' AS o WHERE p.order_id=o.id AND '.$wherep.' GROUP BY order_id');
  $n1 = $n2 = 0;
  if ($rs->numRows() > 0) {
    foreach ($rs as $row) {
      $n1 += ($row['s2'] / $row['s1']);
      $n2 += $row['s2'];
    }
    $n1 = $n1 / $rs->numRows();
    $n2 = $n2 / $rs->numRows();
  }
  $h['montant moyen d’un achat'] = H::decimal($n1);
  $h['montant moyen d’un panier'] = H::decimal($n2);
  
  
  $rs = $dbo->query('SELECT SUM(quantity) AS s'
                    .' FROM '.T('purchase').' AS p,'
                    .     ' '.T('order').' AS o'
                    .' WHERE p.order_id=o.id'
                    .  ' AND '.$wherep
                    .' GROUP BY order_id'); 
  $n = 0;
  if ($rs->numRows() > 0) {
    foreach ($rs as $row) {
      $n += $row['s'];
    }
    $n = $n / $rs->numRows();
  }
  $h['nbre moyen d’articles par panier'] = H::decimal($n);

  $ctx->stats = $h;


  $rs = $dbo->query('SELECT model_id,'
                    .     ' COUNT(*) AS n,'
                    .     ' SUM(quantity) as q,'
                    .     ' SUM(price) as a'
                    .' FROM '.T('purchase').' AS p,'
                    .     ' '.T('order').' AS o'
                    .' WHERE p.order_id=o.id'
                    .  ' AND '.$wherep
                    .' GROUP BY model_id'
                    .' ORDER BY n DESC'
                    .' LIMIT '.$limit); 
  $models = array();
  foreach ($rs as $row) {
    if (!Model::exists($row['model_id'])) continue ;
    $models[] = array(N($row['model_id']), 
                      'commandes='.$row['n'].'<br/>'.
                      'quantité='.$row['q'].'<br/>'.
                      'montant='.H::decimal($row['a'] * $row['q']));
  }
  $ctx->models = $models;

  $rs = $dbo->query('SELECT CONCAT(model_id, "-", record_id) AS item,'
                    .     ' COUNT(*) AS n'
                    .' FROM '.T('purchase').' AS p,'
                    .     ' '.T('order').' AS o'
                    .' WHERE p.order_id=o.id'
                    .  ' AND '.$wherep
                    .' GROUP BY item'
                    .' ORDER BY n DESC'
                    .' LIMIT '.$limit); 
  $items = array();
  foreach ($rs as $row) {
    list ($model_id, $record_id) = explode('-', $row['item']);
    try {
      $item = Item::getInstance($model_id, $record_id, 
                                array('ignore' => true));
      $key = $item->getName();
    }
    catch (Exception $e) { $key = null; }
    $key = $key ?: '# '.N($model_id).' ('.$record_id.')';
    $items[] = array($key, $row['n']);
  }
  $ctx->items = $items;

  $rs = $dbo->query('SELECT CONCAT(p.model_id, "-", p.record_id) AS item,'
                    .     ' SUM(p.price) AS s'
                    .' FROM '.T('purchase').' AS p,'
                    .     ' '.T('order').' AS o'
                    .' WHERE p.order_id=o.id'
                    .  ' AND '.$wherep
                    .' GROUP BY item'
                    .' ORDER BY s DESC'
                    .' LIMIT '.$limit);
  $items = array();
  foreach ($rs as $row) {
    list ($model_id, $record_id) = explode('-', $row['item']);
    try {
      $item = Item::getInstance($model_id, $record_id, 
                                array('ignore' => true));
      $key = $item->getName();
    }
    catch (Exception $e) { $key = null; }
    $key = $key ?: '# '.N($model_id).' ('.$record_id.')';
    $items[] = array($key, H::decimal($row['s']));
  }
  $ctx->ca = $items;

  $rs = $dbo->query('SELECT CONCAT(model_id, "-", record_id) AS item,'
                    .     ' COUNT(p.id) AS n'
                    .' FROM '.T('purchase').' AS p,'
                    .     ' '.T('order').' AS o'
                    .' WHERE p.order_id=o.id'
                    .  ' AND '.$wheres
                    .' GROUP BY item'
                    .' ORDER BY n DESC'
                    .' LIMIT '.$limit); 
  $items = array();
  foreach ($rs as $row) {
    list ($model_id, $record_id) = explode('-', $row['item']);
    try {
      $item = Item::getInstance($model_id, $record_id, 
                                array('ignore' => true));
      $key = $item->getName();
    }
    catch (Exception $e) { $key = null; }
    $key = $key ?: '# '.N($model_id).' ('.$record_id.')';
    $items[] = array($key, $row['n']);
  }
  $ctx->cart = $items;
  
  $rs = $dbo->query('SELECT u.email,'
                    .     ' SUM(o.total_amount) AS s,'
                    .     ' COUNT(o.id) AS n'
                    .' FROM '.T('order').' AS o,'
                    .     ' '.T('user').' AS u'
                    .' WHERE o.user_id=u.id'
                    .  ' AND '.$wherep
                    .' GROUP BY o.user_id'
                    .' ORDER BY s DESC'
                    .' LIMIT '.$limit); 
  $items = array();
  foreach ($rs as $row) {
    $key = $row['email'];
    $value  = H::decimal($row['s']);
    $value .= ' - '.$row['n'].' commande(s)';
    $items[$key] = $value;
  }
  $ctx->users = $items;

  $rs = $dbo->query('SELECT o.payment_id,'
                    .     ' SUM(o.total_amount) AS s,'
                    .     ' COUNT(o.id) AS n'
                    .' FROM '.T('order').' AS o'
                    .' WHERE '.$wherep
                    .' GROUP BY o.payment_id'
                    .' ORDER BY n DESC'); 
  $items = array();
  foreach ($rs as $row) {
    try {
      $key = Payment::getInstance($row['payment_id'])->getName();
    }
    catch (Exception $e) { $key = 'PAYMENT ('.$row['payment_id'].')'; }
    $value  = H::decimal($row['s']);
    $value .= ' - '.$row['n'].' commande(s)';
    $items[$key] = $value;
  }
  $ctx->payments = $items;


  $rs = $dbo->query('SELECT p.country_id,'
                    .     ' SUM(o.total_amount) AS s,'
                    .     ' COUNT(p.country_id) AS n'
                    .' FROM '.T('order').' AS o,'
                    .     ' '.T('person').' AS p'
                    .' WHERE o.user_id=p.user_id'
                    .  ' AND '.$wherep
                    .' GROUP BY p.country_id'
                    .' ORDER BY n DESC'); 
  $items = array();
  foreach ($rs as $row) {
    $key = ucfirst(Country::name($row['country_id']));
    $value  = 'commandes='.$row['n'].'<br/>'.
      'montants='.H::decimal($row['s']);
    $items[$key] = $value;
  }
  $ctx->countries = $items;

}
catch (Exception $e) { $err->add($e); }
 
 