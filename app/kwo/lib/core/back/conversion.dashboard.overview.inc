<?php

try {

  $db = DashBoard::getInstance();

  //--

  $b = $db->getBlock();
  $b->setLabel(l('statistiques générales'));
  $h = array();

  $rs = $dbo->query('SELECT model_id, record_id, worth, span,'
                    .     ' HOUR(converted_at) AS hour, nth_conversion_day, nth_visit_day'
                    .' FROM '.T('conversion')
                    .' WHERE '.$db->filter('converted_at'));
  $models = $hours = $nthvis = $spans = array();
  $first = $days = $s = $qt = $qf = $vd = 0;
  foreach ($rs as $row) {
    $model_id = $row['model_id'];
    $models[$model_id]['n'] += 1;
    $models[$model_id]['w'] += $row['worth'];
    if ($row['nth_conversion_day'] <= 1) $first++;
    $s += $row['worth'];
    $qt++;
    $hours[$row['hour'].'h']++;
    if ($row['nth_conversion_day'] == 1) {
      $spans[$row['span'].' j']++;
      $nthvis[$row['nth_visit_day'].' vis']++;
      if ($row['span'] < 365) {
        $days += $row['span'];
      }
      $vd += $row['nth_visit_day'];
      $qf++;
    }
  }

  $total = $rs->count();

  $h[l('nbre de conversion(s)')] = $qt;
  $h[l('chiffre d’affaires')] = $s;
  if ($qt > 0) {
    $h[l('conversion(s) dès la 1ère visite')] = number_format(($first / $qt) * 100, 1).'%';
  }
  if ($qf > 0) {
    $h[l('délai jusqu’à la 1ère conversion')] = number_format($days / $qf, 1).' '.l('jour(s)');
    $h[l('visites jusqu’à la 1ère conversion')] = number_format($vd / $qf, 1).' '.l('visite(s)');
  }
  $b->setValues($h);

  //--

/*  $h = array();
  foreach ($models as $model_id => $row) {
    $h[] = array(Model::name($model_id), $row['n'], $row['w']);
  } */
  if (!empty($models)) {
    $b = $db->getBlock();
    $b->setLabel(l('types de conversions'));
    $b->setColumns(array('model_id' => l('type'),
                         'n' => array('width' => '40px',
                                      'label' => l('nbre')),
                         'w' => l('valeur')));
    $h = array();
    foreach ($models as $model_id => $row) {
      $h[] = array(Model::name($model_id), $row['n'], $row['w']);
    }
    $b->setValues($h);
  }

  //--

  $rs = $dbo->query('SELECT medium, COUNT(id) AS n, SUM(worth) AS w, SUM(cost) AS cs'
                    .' FROM '.T('conversion')
                    .' WHERE '.$db->filter('converted_at')
                    .  ' AND medium>0'
                    .' GROUP BY medium'
                    .' ORDER BY n DESC');
  if ($rs->count() > 0) {
    $b = $db->getBlock();
    $b->setLabel(l('origine des conversions'));
    $b->setColumns(array('medium' => l('medium'),
                         'n' => array('width' => '40px',
                                      'label' => l('nbre')),
                         'w' => l('gains'),
                         'cs' => l('coûts')));
    foreach ($rs as $row) {
      $values = array(Tracker::$mediums[$row['medium']],
                      $row['n'],
                      $row['w'],
                      $row['cs']);
      if ($row['medium'] != 1) {
        $b->addRow(array('values' => $values,
                         'action' => 'conversion.dashboard.medium',
                         'params' => array('medium' => $row['medium'])));
      }
      else {
        $b->addRow(array('values' => $values));
      }
    }
  }

  //--

  $rs = $dbo->query('SELECT u.name, COUNT(c.id) AS n, SUM(c.worth) AS w'
                    .' FROM '.T('conversion').' AS c,'
                    .     ' '.T('url').' AS u,'
                    .     ' '.T('visit').' AS v'
                    .' WHERE '.$db->filter('c.converted_at')
                    .  ' AND c.visit_id=v.id'
                    .  ' AND v.url_id>0'
                    .  ' AND v.url_id=u.id'
                    .' GROUP BY v.url_id'
                    .' ORDER BY n DESC');
  if ($rs->count() > 0) {
    $b = $db->getBlock();
    $b->setLabel(l('pages de destination'));
    $b->setColumns(array('name' => l('url'),
                         'n' => array('width' => '40px',
                                      'label' => l('nbre')),
                         'w' => array('width' => '70px',
                                      'label' => l('gains'))));
    foreach ($rs as $row) {
      $row['name'] = str_replace('http://', '', $row['name']);
      $values = array($row['name'],
                      $row['n'],
                      $row['w']);
      $b->addRow(array('values' => $values));
    }
  }

  //--

  arsort($hours);
  $b = $db->getBlock();
  $b->setLabel(l('heures les plus actives'));
  $b->setValues($hours);

  //--

  arsort($nthvis);
  $b = $db->getBlock();
  $b->setLabel(l('nbre de visites jusqu’à la 1ère conversion'));
  $b->setValues($nthvis);

  //--

  arsort($spans);
  $b = $db->getBlock();
  $b->setLabel(l('nbre de jours jusqu’à la 1ère conversion'));
  $b->setValues($spans);

  //--

  $rs = $dbo->query('SELECT v.locale, COUNT(c.id) AS n, SUM(c.worth) AS w'
                    .' FROM '.T('conversion').' AS c,'
                    .     ' '.T('visit').' AS v'
                    .' WHERE c.visit_id=v.id'
                    .  ' AND '.$db->filter('c.converted_at')
                    .' GROUP BY v.locale'
                    .' ORDER BY n DESC');
  if ($rs->count() > 1) {
    $b = $db->getBlock();
    $b->setLabel(l('locales / langues'));
    $b->setColumns(array('locale' => l('langues'),
                         'n' => array('width' => '40px',
                                      'label' => l('nbre')),
                         'w' => l('gains')));
    foreach ($rs as $row) {
      $values = array(Locale::toCode($row['locale']), //$values = array(I18N::locale2code($row['locale']),
                      $row['n'],
                      $row['w']);
      $b->addRow(array('values' => $values));
    }
  }

  //--

  $rs = $dbo->query('SELECT v.country, COUNT(c.id) AS n, SUM(c.worth) AS w'
                    .' FROM '.T('conversion').' AS c,'
                    .     ' '.T('visit').' AS v'
                    .' WHERE c.visit_id=v.id'
                    .  ' AND '.$db->filter('c.converted_at')
                    .' GROUP BY v.country'
                    .' ORDER BY n DESC');
  if ($rs->count() > 1) {
    $b = $db->getBlock();
    $b->setLabel(l('pays'));
    $b->setColumns(array('country' => l('pays'),
                         'n' => array('width' => '40px',
                                      'label' => l('nbre')),
                         'w' => l('gains')));
    foreach ($rs as $row) {
      $values = array(Country::code($row['country']),
                      $row['n'],
                      $row['w']);
      $b->addRow(array('values' => $values));
    }
  }

  //--

  $res->write($db);

}
catch (Exception $e) { $err->add($e); }