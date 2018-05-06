<?php

function h2a($h) {
  $a = array();
  if (empty($h)) return $h;
  arsort($h);
  foreach ($h as $key => $value) {
    $a[] = array($key, $value);
  }
  return $a;
}

$ttl = 20; //minutes

try {

  $struct = array('visitors' => 0,
                  'visits' => 0,
                  'newcomers' => 0,
                  'countries' => array(),
                  'locales' => array(),
                  'mediums' => array(),
                  'referers' => array(),
                  'queries' => array(),
                  'users' => array(),
                  'socials' => array());

  $apps = array(1 => $struct);

  $rs = $dbo->query('SELECT visit_id, action, category'
                    .' FROM '.Tracker::TABLE_TMP
                    .' WHERE created_at>"'.DH::when($ttl * -60).'"'
                    .' GROUP BY visit_id');
  $visits = $pages = array();
  foreach ($rs as $row) {
    $visits[] = (int) $row['visit_id'];
    $name = $row['category'].' > '.$row['action'];
    $pages[$name]++;
  }

  $visits = empty($visits) ? array(0) : $visits;
  $rs = $dbo->query('SELECT id, app_id, number, country, locale, medium, source, term, user_id'
                    .' FROM '.Visit::TABLE
                    .' WHERE id IN ('.join(',', $visits).')');
  $exprs = array();
  foreach ($rs as $row) {
    if ($row['source'] > 0) {
      $exprs[$row['source']] = 1;
    }
    if ($row['term'] > 0) {
      $exprs[$row['term']] = 1;
    }
  }
  if (count($exprs) > 0) {
    $exprs = $dbo->asHash('SELECT id, name'
                          .' FROM '.Tracker::TABLE_EXPRESSION
                          .' WHERE id IN ('.join(',', array_keys($exprs)).')');
  }

  $app_id = 1;

  foreach ($rs as $row) {
    if (!isset($apps[$app_id])) {
      $apps[$app_id] = $struct;
    }
    $apps[$app_id]['visitors']++;
    if ($row['number'] == 1) $apps[$app_id]['newcomers']++;
    if ($row['user_id'] > 0) {
      $email = $dbo->fetchValue('SELECT email'
                                .' FROM '.T('user')
                                .' WHERE id='.$row['user_id']
                                .' LIMIT 1');
      $apps[$app_id]['users'][$email]++;
    }
    $apps[$app_id]['countries'][Country::code($row['country'])]++;
    $apps[$app_id]['locales'][Locale::toCode($row['locale'])]++;
    $apps[$app_id]['mediums'][l(Tracker::$mediums[$row['medium']])]++;
    if ($row['medium'] == Tracker::MEDIUM_SEARCH && $row['term'] > 0) {
      $apps[$app_id]['queries'][$exprs[$row['term']]]++;
    }
    if ($row['medium'] == Tracker::MEDIUM_LINK && $row['source'] > 0) {
      $apps[$app_id]['referers'][$exprs[$row['source']]]++;
    }
    if ($row['medium'] == Tracker::MEDIUM_SOCIAL && $row['source'] > 0) {
      $apps[$app_id]['socials'][$exprs[$row['source']]]++;
    }
  }

  $apps[$app_id]['pages'] = $pages;
  $apps[$app_id]['visits'] = $dbo->fetchValue('SELECT COUNT(id) FROM '.T('visit').' WHERE created_on="'.DH::today().'"');

  foreach ($apps as $app_id => $h) {
    foreach ($h as $key => $value) {
      if (is_array($value)) {
        $apps[$app_id][$key] = h2a($value);
      }
    }
  }

  $ctx->apps = $apps;

}
catch (Exception $e) { $err->add($e); }