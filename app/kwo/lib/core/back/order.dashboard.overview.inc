<?php

try {

  $limit = 20;

  $db = DashBoard::getInstance();

  $wheres = $db->filter('o.started_at');
  $wherep = $db->filter('o.paid_at').' AND o.status>='.Order::STATUS_PAID;

  $b = $db->getBlock();
  $b->setLabel(l('statistiques e-commerce générales'));
  $h = array();

  $n = $dbo->fetchValue('SELECT SUM(total_amount) FROM '.T('order').' AS o WHERE '.$wherep);
  $h[l('chiffre d’affaire TTC')] = (int) $n;

  $n = $dbo->fetchValue('SELECT count(*) FROM '.T('order').' AS o WHERE '.$wherep);
  $h[l('nbre de commandes payées')] = (int) $n;

  $n = $dbo->fetchValue('SELECT count(*) FROM '.T('order').' AS o WHERE '.$wheres);
  $h[l('nbre de paniers')] = (int) $n;

  $n = $dbo->fetchValue('SELECT COUNT(user_id) FROM '.T('order').' AS o WHERE '.$wherep);
  $h[l('nbre de clients')] = (int) $n;

  $n = $dbo->fetchValue('SELECT count(DISTINCT user_id) FROM '.T('order').' AS o WHERE '.$wherep);
  $h[l('nbre de clients différents')] = (int) $n;

  $n = $dbo->fetchValue('SELECT SUM(discount_amount) FROM '.T('order').' AS o WHERE '.$wherep);
  $h[l('montant des réductions')] = (int) $n;

  $rs = $dbo->query('SELECT DISTINCT(concat(model_id, "-", record_id)) FROM '.T('purchase').' AS p, '.T('order').' AS o WHERE p.order_id=o.id AND '.$wherep);
  $h[l('nbre d’articles différents vendus')] = (int) $rs->numRows();

  $rs = $dbo->query('SELECT SUM(quantity) AS s1, SUM(quantity*price) AS s2 FROM '.T('purchase').' AS p, '.T('order').' AS o WHERE p.order_id=o.id AND '.$wherep.' GROUP BY order_id');
  $n1 = $n2 = 0;
  if ($rs->numRows() > 0) {
    foreach ($rs as $row) {
      $n1 += ($row['s2'] / $row['s1']);
      $n2 += $row['s2'];
    }
    $n1 = $n1 / $rs->numRows();
    $n2 = $n2 / $rs->numRows();
  }
  $h[l('montant moyen d’un achat')] = H::decimal($n1);
  $h[l('montant moyen d’un panier')] = H::decimal($n2);

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
  $hash[l('nbre moyen d’articles par panier')] = H::decimal($n);

  $b->setValues($h);

  /*** model_id ***/

  $rs = $dbo->query('SELECT model_id,'
                    .     ' COUNT(DISTINCT order_id) AS n,'
                    .     ' SUM(quantity) as q,'
                    .     ' SUM(price*quantity) as a'
                    .' FROM '.T('purchase').' AS p,'
                    .     ' '.T('order').' AS o'
                    .' WHERE p.order_id=o.id'
                    .  ' AND '.$wherep
                    .' GROUP BY p.model_id'
                    .' ORDER BY n DESC');
  $h = array();
  foreach ($rs as $row) {
    $model = Model::exists($row['model_id'])
           ? Model::name($row['model_id'])
           : 'model('.$row['model_id'].')';
    $h[] = array($model,
                 $row['n'],
                 $row['q'],
                 H::decimal($row['a']));
  }
  if (!empty($h) && count($h) > 1) {
    $b = $db->getBlock();
    $b->setLabel(l('types d’articles les plus vendus'));
    $b->setColumns(array('model_id' => 'type',
                         'n' => array('width' => '80px',
                                      'label' => c(l('nbre cmdes'))),
                         'q' => array('width' => '40px',
                                      'label' => l('qté')),
                         'a' => array('label' => l('CA'))));
    $b->setValues($h);
  }

  /*** brand ***/
  /*** TODO : détail sur une marque ***/
  $rs = $dbo->query('SELECT brand,'
                    .     ' COUNT(DISTINCT order_id) AS n,'
                    .     ' SUM(quantity) as q,'
                    .     ' SUM(price*quantity) as a'
                    .' FROM '.T('purchase').' AS p,'
                    .     ' '.T('order').' AS o'
                    .' WHERE p.order_id=o.id'
                    .  ' AND LENGTH(brand)>0'
                    .  ' AND '.$wherep
                    .' GROUP BY p.brand'
                    .' ORDER BY n DESC');
  $h = array();
  foreach ($rs as $row) {
    $h[] = array($row['brand'],
                 $row['n'],
                 $row['q'],
                 H::decimal($row['a']));
  }
  if (!empty($h)) {
    $b = $db->getBlock();
    $b->setLabel(l('marques les plus vendues'));
    $b->setColumns(array('brand' => l('marque'),
                         'n' => array('width' => '80px',
                                      'label' => c(l('nbre cmdes'))),
                         'q' => array('width' => '40px',
                                      'label' => l('qté')),
                         'a' => array('label' => l('CA'))));
    $b->setValues($h);
  }

  /*** category ***/
  /*** TODO : détail sur une category ***/
  $rs = $dbo->query('SELECT category,'
                    .     ' COUNT(DISTINCT order_id) AS n,'
                    .     ' SUM(quantity) as q,'
                    .     ' SUM(price*quantity) as a'
                    .' FROM '.T('purchase').' AS p,'
                    .     ' '.T('order').' AS o'
                    .' WHERE p.order_id=o.id'
                    .  ' AND LENGTH(category)>0'
                    .  ' AND '.$wherep
                    .' GROUP BY p.category'
                    .' ORDER BY n DESC');
  $h = array();
  foreach ($rs as $row) {
    $h[] = array($row['category'],
                 $row['n'],
                 $row['q'],
                 H::decimal($row['a']));
  }
  if (!empty($h)) {
    $b = $db->getBlock();
    $b->setLabel(l('catégories les plus vendues'));
    $b->setColumns(array('category' => l('catégorie'),
                         'n' => array('width' => '80px',
                                      'label' => c(l('nbre cmdes'))),
                         'q' => array('width' => '40px',
                                      'label' => l('qté')),
                         'a' => array('label' => l('CA'))));
    $b->setValues($h);
  }

  /*** customs ***/
  $rs = $dbo->query('SELECT p.model_id, p.quantity AS q, (p.price*p.quantity) AS a, p.order_id,'
                    .     ' p.custom1, p.custom2, p.custom3, p.custom4, p.custom5'
                    .' FROM '.T('purchase').' AS p,'
                    .     ' '.T('order').' AS o'
                    .' WHERE p.order_id=o.id'
                    .  ' AND '.$wherep
                    .' ORDER BY order_id ASC');
  $h = array();
  foreach ($rs as $row) {
    $model_id = $row['model_id'];
    if (!is_array($h[$model_id])) {
      $h[$model_id] = array();
      for ($i = 1; $i <= 5; $i++) {
        $h[$model_id]['custom'.$i] = array();
      }
    }
    for ($i = 1; $i <= 5; $i++) {
      $var = 'custom'.$i;
      $val = $row[$var];
      if (empty($val)) continue ;
      if (empty($h[$model_id][$var][$val])) {
        // compter le nombre de commandes
        $h[$model_id][$var][$val] = array('name' => $val,
                                          'n' => 0,
                                          'q' => 0,
                                          'a' => 0,
                                          'last_order_id' => 0);
      }
      if ($h[$model_id][$var][$val]['last_order_id'] != $row['order_id']) {
        $h[$model_id][$var][$val]['last_order_id'] = $row['order_id'];
        $h[$model_id][$var][$val]['n'] += 1;
      }
      $h[$model_id][$var][$val]['q'] += $row['q'];
      $h[$model_id][$var][$val]['a'] += $row['a'];
    }
  }
  foreach ($h as $model_id => $customs) {
    for ($i = 1; $i <= 5; $i++) {
      $var = 'custom'.$i;
      if (empty($customs[$var])) continue ;
      $rows = $customs[$var];
      $b = $db->getBlock();
      $b->setLabel(Model::name($model_id).': '.$var);
      $b->setColumns(array('name' => l('intitulé'),
                           'n' => array('width' => '80px',
                                        'label' => c(l('nbre cmdes'))),
                           'q' => array('width' => '40px',
                                        'label' => l('qté')),
                           'a' => array('label' => l('CA'))));
      $b->setValues($rows);
    }
  }

  /*** ***/

  $rs = $dbo->query('SELECT CONCAT(p.model_id, "-", p.record_id) AS item,'
                    .     ' SUM(p.quantity) AS n'
                    .' FROM '.T('purchase').' AS p,'
                    .     ' '.T('order').' AS o'
                    .' WHERE p.order_id=o.id'
                    .  ' AND '.$wherep
                    .' GROUP BY item'
                    .' ORDER BY n DESC'
                    .' LIMIT '.$limit);
  $h = array();
  foreach ($rs as $row) {
    list ($model_id, $record_id) = explode('-', $row['item']);
    try {
      $item = Item::getInstance($model_id, $record_id,
                                array('ignore' => true));
      $key = $item->getName();
    }
    catch (Exception $e) { $key = null; }
    $model = Model::exists($model_id)
           ? Model::name($model_id)
           : 'model('.$model_id.')';
    $key = $key ?: '** '.$model.' ('.$record_id.') **';
    $h[] = array($key, $row['n']);
  }
  if (!empty($h) && count($h) > 1) {
    $b = $db->getBlock();
    $b->setLabel(l('articles les plus vendus'));
    $b->setValues($h);
  }

  $rs = $dbo->query('SELECT CONCAT(p.model_id, "-", p.record_id) AS item,'
                    .     ' SUM(p.price) AS s'
                    .' FROM '.T('purchase').' AS p,'
                    .     ' '.T('order').' AS o'
                    .' WHERE p.order_id=o.id'
                    .  ' AND '.$wherep
                    .' GROUP BY item'
                    .' ORDER BY s DESC'
                    .' LIMIT '.$limit);
  $h = array();
  foreach ($rs as $row) {
    list ($model_id, $record_id) = explode('-', $row['item']);
    try {
      $item = Item::getInstance($model_id, $record_id,
                                array('ignore' => true));
      $key = $item->getName();
    }
    catch (Exception $e) { $key = null; }
    $model = Model::exists($model_id)
           ? Model::name($model_id)
           : 'model('.$model_id.')';
    $key = $key ?: '** '.$model.' ('.$record_id.') **';
    $h[] = array($key, H::decimal($row['s']));
  }
  if (!empty($h) && count($h) > 1) {
    $b = $db->getBlock();
    $b->setLabel(l('articles générant le plus de CA'));
    $b->setValues($h);
  }

  $rs = $dbo->query('SELECT CONCAT(model_id, "-", record_id) AS item,'
                    .     ' COUNT(p.id) AS n'
                    .' FROM '.T('purchase').' AS p,'
                    .     ' '.T('order').' AS o'
                    .' WHERE p.order_id=o.id'
                    .  ' AND '.$wheres
                    .' GROUP BY item'
                    .' ORDER BY n DESC'
                    .' LIMIT '.$limit);
  $h = array();
  foreach ($rs as $row) {
    list ($model_id, $record_id) = explode('-', $row['item']);
    try {
      $item = Item::getInstance($model_id, $record_id,
                                array('ignore' => true));
      $key = $item->getName();
    }
    catch (Exception $e) { $key = null; }
    $model = Model::exists($model_id)
           ? Model::name($model_id)
           : 'model('.$model_id.')';
    $key = $key ?: '** '.$model.' ('.$record_id.') **';
    $h[] = array($key, $row['n']);
  }
  if (!empty($h) && count($h) > 1) {
    $b = $db->getBlock();
    $b->setLabel(l('articles les plus ajoutés au panier'));
    $b->setValues($h);
  }

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
  $h = array();
  foreach ($rs as $row) {
    $h[] = array($row['email'],
                  H::decimal($row['s']),
                  $row['n']);
  }
  if (!empty($h) && count($h) > 1) {
    $b = $db->getBlock();
    $b->setLabel(l('meilleurs clients'));
    $b->setColumns(array('email' => l('client'),
                         's' => array('width' => '80px',
                                      'label' => c(l('montant'))),
                         'n' => array('width' => '50px',
                                      'label' => l('cmdes'))));
    $b->setValues($h);
  }

  $rs = $dbo->query('SELECT p.method_id,'
                    .     ' SUM(p.amount) AS s,'
                    .     ' COUNT(o.id) AS n'
                    .' FROM '.T('order').' AS o, '
                    .         T('payment').' AS p'
                    .' WHERE '.$wherep
                    .  ' AND p.order_id=o.id'
                    .  ' AND p.status='.Payment::STATUS_RECEIVED
                    .' GROUP BY p.method_id'
                    .' ORDER BY n DESC');
  $h = array();
  foreach ($rs as $row) {
    try {
      $key = Method::getInstance($row['method_id'])->getName();
    }
    catch (Exception $e) {
      $key = '** method ('.$row['method_id'].') **';
    }
    $h[] = array($key, H::decimal($row['s']), $row['n']);
  }
  if (!empty($h)) {
    $b = $db->getBlock();
    $b->setLabel(l('moyens de paiements'));
    $b->setColumns(array('method_id' => l('type'),
                         's' => array('width' => '80px',
                                      'label' => c(l('montant'))),
                         'n' => array('width' => '50px',
                                      'label' => l('cmdes'))));
    $b->setValues($h);
  }

  $rs = $dbo->query('SELECT p.country_id,'
                    .     ' SUM(o.total_amount) AS s,'
                    .     ' COUNT(p.country_id) AS n'
                    .' FROM '.T('order').' AS o, '
                    .         T('person').' AS p'
                    .' WHERE o.user_id=p.user_id'
                    .  ' AND '.$wherep
                    .' GROUP BY p.country_id'
                    .' ORDER BY n DESC');
  $h = array();
  foreach ($rs as $row) {
    $key = ucfirst(Country::name($row['country_id'], BackManager::getLocale()));
    $h[] = array($key, H::decimal($row['s']), $row['n']);
  }
  if (!empty($h)) {
    $b = $db->getBlock();
    $b->setLabel(l('pays'));
    $b->setColumns(array('country_id' => l('pays'),
                         's' => array('width' => '80px',
                                      'label' => c(l('montant'))),
                         'n' => array('width' => '50px',
                                      'label' => l('cmdes'))));
    $b->setValues($h);
  }

  /*** locales ***/

  $rs = $dbo->query('SELECT o.locale,'
                    .     ' SUM(o.total_amount) AS s,'
                    .     ' COUNT(o.locale) AS n'
                    .' FROM '.T('order').' AS o'
                    .' WHERE '.$wherep
                    .' GROUP BY o.locale'
                    .' ORDER BY n DESC');
  $h = array();
  foreach ($rs as $row) {
    $key = ucfirst(Locale::toCode($row['locale']));
    $h[] = array($key, H::decimal($row['s']), $row['n']);
  }
  if (count($h) > 0) {
    $b = $db->getBlock();
    $b->setLabel(l('langues / locales'));
    $b->setColumns(array('locale' => l('locale'),
                         's' => array('width' => '80px',
                                      'label' => c(l('montant'))),
                         'n' => array('width' => '50px',
                                      'label' => l('cmdes'))));
    $b->setValues($h);
  }


  $res->write($db);

}
catch (Exception $e) { $err->add($e); }