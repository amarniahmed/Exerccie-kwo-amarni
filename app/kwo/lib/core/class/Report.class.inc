<?php

class Report {

  const TABLE = 'core_tracker_report';

  private $attributes = array();
  private $dbo = null;
  private $log = null;

  public $visits = 0;

  private $period = null;

  private $db = null;

  public static $dimensions = array('browser' => 1, 'campaign' => 1, 'community' => 2, 'conversion' => 3, 'country' => 1,
                                    'depth' => 1, 'event' => 3, 'exit' => 1, 'hour' => 1, 'interaction' => 3,
                                    'landing' => 1, 'locale' => 1, 'medium' => 3, 'metric' => 1,
                                    'path' => 1, 'search' => 1, 'share' => 1, 'source' => 1, 'system' => 1, 'term' => 1);

  public static $ages = array(1 => array(0,  7),
                              2 => array(8,  12),
                              3 => array(13, 18),
                              4 => array(19, 23),
                              5 => array(24, 30),
                              6 => array(31, 40),
                              7 => array(41, 50),
                              8 => array(51, 65),
                              9 => array(65, 150));


  public function __construct($period) {
    if ($period instanceof DashBoard) {
      $this->db = $period;
      $period = $period->getPeriod();
    }
    $this->period = $period;
    $this->dbo = S('dbo');
    list ($this->year, $this->month, $this->day) = explode('-', $period);
    $h = $this->dbo->fetchHash('SELECT *'
                               .' FROM '.self::TABLE
                               .' WHERE year='.$this->year
                               .  ' AND month='.$this->month
                               .  ' AND day='.$this->day);
    if ($h === false || $h === null) {
      foreach (self::$dimensions as $dimension) {
        $this->attributes[$dimension] = array();
      }
      return ;
    }
    foreach ($h as $key => $value) {
      if (!array_key_exists($key, self::$dimensions)) continue ;
      $this->attributes[$key] = empty($value) ? array() : unserialize($value);
    }
    $this->visits = $this->attributes['metric']['visit'];
    if ($this->db === null) return ;
    $this->addBlocks();
  }

  private function addBlocks() {
    $this->addGraphBlock();
    $this->addMetricsBlock();
    $this->addCommunityBlock();
    $this->addMediumsBlock();
    $this->addConversionsBlock();
    $this->addInteractionsBlock();
    $this->addEventsBlock();
  }

  private function addGraphBlock() {
    $b = $this->db->getBlock();
    $b->setGraph(H::url('/back/core/chart',
                        array('year' => $this->year,
                              'month' => $this->month,
                              'day' => $this->day,
                              'width' => '360',
                              'height' => '120',
                              'ts' => time())));
  }

  private function addMetricsBlock() {
    $b = $this->db->getBlock();
    $b->setLabel(l('visites'));
    $metrics = $this->getAttribute('metric');
    $h = array();
    $visit_id = $this->getLowestVisitId();
    if ($visit_id > 0) {
      $h[l('visites')] = Visit::link($visit_id, (int) $metrics['visit']);
    }
    else {
      $h[l('visites')] = 0;
    }
    if ($metrics['visit'] > 0) {
      $h[l('visiteurs uniques')] = $metrics['visitor'];
      $h[l('tx habitués')] = $metrics['returning'].'%';
      $h[l('nouveaux visiteurs')] = (int) $metrics['new'];
      $h[l('tx rebond')] = $metrics['bounce'].'%';
      $h[l('pages vues')] = $metrics['page'];
      $h[l('interactions')] = $metrics['interaction'];
      $h[l('conversions')] = $metrics['conversion'];
      $b->addAction('<span class="icon-zoom-in"></span>', 'visit.dashboard.metrics');
    }
    $b->setValues($h);
  }

  private function getLowestVisitId() {
    return S('dbo')->fetchValue('SELECT MIN(id)'
                                .' FROM '.Visit::TABLE
                                .' WHERE '.$this->getSqlCondition('created_on')
                                .' LIMIT 1');
  }

  private function addCommunityBlock() {
    $b = $this->db->getBlock();
    $b->setLabel(l('communauté'));
    $metrics = $this->getAttribute('metric');
    $h = array();
    $h[l('nouveaux utilisateurs')] = (int) $metrics['user'];
    $h[l('visites authentifiées')] = $this->format($metrics['person']);
    $b->setValues($h);
    if ($metrics['user'] > 0) {
      $b->addAction('<span class="icon-zoom-in"></span>', 'visit.dashboard.community');
    }
  }

  private function addMediumsBlock() {
    $b = $this->db->getBlock();
    $b->setLabel(l('sources de trafic'));
    $h = array();
    $mediums = $this->getAttribute('medium');
    foreach ($mediums as $key => $value) {
      $values = array(l(Tracker::$mediums[$key]),
                      $value.'%');
      $row['values'] = $values;
      if ($value > 0) {
        $row['action'] = 'visit.dashboard.medium';
        $row['params'] = array('medium_id' => $key);
      }
      $b->addRow($row);
    }
  }

  private function addConversionsBlock() {
    $conversions = $this->getAttribute('conversion');
    if (empty($conversions)) return ;
    $b = $this->db->getBlock();
    $b->setLabel(l('conversions'));
    $h = array();
    foreach ($conversions as $conversion) {
      $values = array($conversion['name'],
                      $conversion['count']);
      $row['values'] = $values;
      if ($conversion['count'] > 0) {
        $row['action'] = 'visit.dashboard.conversion';
        $row['params'] = array('model_id' => $conversion['id']);
      }
      $b->addRow($row);
    }
  }

  private function addInteractionsBlock() {
    $b = $this->db->getBlock();
    $b->setLabel(l('interactions'));
    $h = array();
    $interactions = $this->getAttribute('interaction');
    foreach ($interactions as $interaction) {
      if (!Model::exists((int) $interaction['id'])) continue ;
      $values = array($interaction['name'],
                      $interaction['count']);
      $row['values'] = $values;
      $row['action'] = 'visit.dashboard.interaction';
      $row['params'] = array('model_id' => $interaction['id']);
      $b->addRow($row);
    }
  }

  private function addEventsBlock() {
    $b = $this->db->getBlock();
    $b->setLabel(l('consultations'));
    $h = array();
    $events = $this->getAttribute('event');
    foreach ($events as $event) {
      $values = array($event['name'],
                      $event['count']);
      $row['values'] = $values;
      if ($event['count'] > 0) {
        $row['action'] = 'visit.dashboard.category';
        $row['params'] = array('category_id' => $event['id']);
      }
      $b->addRow($row);
    }
  }

  public function getMedium($medium_id) {
    $out = array('source' => array(),
                 'term' => array(),
                 'campaign' => array());
    $h = $this->attributes['medium'][$medium_id];
    $count = $h['metric']['count'];
    $strings = array_merge(array_keys((array) $h['campaign']),
                           array_keys((array) $h['source']),
                           array_keys((array) $h['term']));
    $strings = $this->getStrings($strings);
    foreach ($h as $type => $values) {
      if ($type === 'metric') continue ;
      foreach ($values as $id => $n) {
        $source = $strings[$id];
        $source = Item::asSourceName($source);
        $out[$type][$source] = array($source,
                                     number_format(($n / $count) * 100, 1).'%',
                                     $n);
      }
    }
    return $out;
  }

  public function getConversion($model_id) {
    $out = array('medium' => array(),
                 'source' => array(),
                 'term' => array(),
                 'campaign' => array());
    $h = $this->attributes['conversion'][$model_id];
    $count = (int) $h['metric']['count'];
    if ($count < 1) return $out;
    $strings = array_merge(array_keys((array) $h['campaign']),
                           array_keys((array) $h['source']),
                           array_keys((array) $h['term']));
    $strings = $this->getStrings($strings);
    foreach ($h as $type => $values) {
      foreach ($values as $id => $n) {
        if ($id == 'metric') continue ;
        if ($type === 'medium') {
          $name = l(Tracker::$mediums[$id]);
        }
        else {
          $name = $strings[$id];
        }
        $out[$type][$name] = number_format(($n / $count) * 100, 1);
      }
    }
    return $out;
  }

  public function getInteraction($model_id) {
    $h = $this->attributes['interaction'][$model_id];
    $set = array();
    foreach ($h as $type => $struct) {
      foreach ($struct as $record_id => $n) {
        $set[] = $record_id;
      }
    }
    $model_name = Model::name($model_id);
    $names = $this->dbo->asHash('SELECT id, '.Model::fieldName($model_name)
                                .' FROM '.T($model_name)
                                .' WHERE id IN ([set])',
                                array('[set]' => $set));
    $grids = array();
    foreach (Interaction::$names as $type => $name) {
      if (!is_array($h[$type])) continue ;
      $grid = array();
      foreach ($h[$type] as $record_id => $n) {
        $s = $names[$record_id];
        $s = $s ?: '** '.l('inconnu').' **';
        $grid[] = array('id' => $record_id,
                        'name' => $s,
                        'value' => $n);
      }
      $grids[$model_name.' : <small>'.l($name).'</small>'] = $grid;
    }
    return $grids;
  }

  public function getCommunity() {
    $out = array('sexes' => array(),
                 'ages' => array(),
                 'origins' => array(),
                 'logins' => array(),
                 'interactions' => array(),
                 'auths' => array());
    // TODO : ajouter conversions
    $n = $this->attributes['metric']['user'];
    $out['logins'] = S('dbo')->asArray('SELECT u.id, u.email, COUNT(i.record_id) AS n'
                                       .' FROM '.T('user').' AS u,'
                                       .     ' '.Interaction::TABLE.' AS i'
                                       .' WHERE i.type='.Interaction::TYPE_AUTH
                                       .  ' AND i.model_id='.Model::id('user')
                                       .  ' AND i.record_id>0'
                                       .  ' AND i.record_id=u.id'
                                       .  ' AND '.$this->getSqlCondition('i.created_on')
                                       .' GROUP BY i.record_id'
                                       .' HAVING n > 1'
                                       .' ORDER BY n DESC'
                                       .' LIMIT 50');
    $field = BackManager::getLocale() == I18N::LOCALE_FR ? 'name_fr' : 'name_en';
    $out['countries'] = S('dbo')->asHash('SELECT c.'.$field.', COUNT(v.country) AS n'
                                         .' FROM '.Interaction::TABLE.' AS i,'
                                         .     ' '.Visit::TABLE.' AS v,'
                                         .     ' '.T('country').' AS c'
                                         .' WHERE i.type='.Interaction::TYPE_AUTH
                                         .  ' AND i.model_id='.Model::id('user')
                                         .  ' AND i.record_id>0'
                                         .  ' AND i.visit_id=v.id'
                                         .  ' AND c.id=v.country'
                                         .  ' AND '.$this->getSqlCondition('i.created_on')
                                         .' GROUP BY v.country'
                                         .' HAVING n > 1'
                                         .' ORDER BY n DESC'
                                         .' LIMIT 10');
    $out['interactions'] = S('dbo')->asArray('SELECT u.id, u.email, COUNT(i.user_id) AS n'
                                        .' FROM '.T('user').' AS u,'
                                        .     ' '.Interaction::TABLE.' AS i'
                                        .' WHERE i.type<>'.Interaction::TYPE_DISPLAY
                                        .  ' AND i.type<>'.Interaction::TYPE_AUTH
                                        .  ' AND i.user_id>0'
                                        .  ' AND i.user_id=u.id'
                                        .  ' AND '.$this->getSqlCondition('i.created_on')
                                        .' GROUP BY i.user_id'
                                        .' HAVING n > 1'
                                        .' ORDER BY n DESC'
                                        .' LIMIT 50');
    $auths = S('dbo')->asArray('SELECT i.value, COUNT(i.value) AS n'
                               .' FROM '.Interaction::TABLE.' AS i'
                               .' WHERE i.type='.Interaction::TYPE_AUTH
                               .  ' AND i.value>0'
                               .  ' AND '.$this->getSqlCondition('i.created_on')
                               .' GROUP BY i.value');
    foreach ($auths as $auth) {
      $name = AuthManager::$auth_types[$auth['value']];
      $out['auths'][$name] = $auth['n'];
    }
    if ($n < 1) return $out;
    $h = $this->attributes['community'];
    foreach (self::$ages as $id => $b) {
      $value = $h['age'][$id];
      if ($value < 1) continue ;
      if ($b[0] < 1) $name = ' < '.$b[1];
      elseif ($b[1] > 90) $name = ' > '.$b[0];
      else $name = $b[0].'-'.$b[1];
      $out['ages'][$name] = number_format(($value / $n) * 100, 1);
    }
    $origins = Model::asHash('origin', 'name');
    foreach ($h['origin'] as $id => $value) {
      $name = l($origins[$id]);
      $out['origins'][$name] = number_format(($value / $n) * 100, 1);
    }
    $women = $this->attributes['metric']['woman'];
    if ($women != 0 && $women != $n) {
      $out['sexes'] = array('homme' => $n - $women,
                            'femme' => (int) $women);
    }
    return $out;
  }

  public function getCategory($category_id) {
    $out = array();
    $struct = $this->attributes['event'][$category_id];
    $struct['action'] = (array) $struct['action'];
    $count = (int) $struct['metric']['count'];
    $strings = $this->getStrings(array_keys($struct['action']));
    foreach ($struct['action'] as $id => $n) {
      $out[] = array($strings[$id], number_format(($n / $count) * 100, 1).'%', $n);
    }
    return $out;
  }

  private function sortName($a, $b) {
    return $a['name'] < $b['name'] ? -1 : 1;
  }

  public function format($value) {
    if ($this->visits < 1) return '-';
    $n = $value / $this->visits;
    return number_format($n * 100, 1).'%';
  }

  public function getAttribute($name) {
    $h = $this->attributes[$name];
    if ($this->visits < 1) return array();
    if ($name == 'metric') {
      $out = $h;
      $n = $h['visit'];
      $diff = $n - $h['bounce'];
      $diff = $diff > 0 ? $diff : 1;
      $out['returning'] = number_format(($h['returning'] / $n) * 100, 1);
      $out['bounce'] = number_format(($h['bounce'] / $n) * 100, 1);
      $out['dpv'] = intval($out['duration'] / $diff / 60).' mn ';
      if ($out['duration'] % 60) {
        $out['dpv'] .= ($out['duration'] % 60).' s';
      }
      $ppv = round(($h['page'] - $h['bounce']) / $diff);
      $out['ppv'] = $ppv >= 0 ? $ppv : 0;
      $out['ppv'] = $out['ppv'] > 0 ? $out['ppv'] : 1;
      $out['dpp'] = 0;
      if (($h['page'] - $h['bounce']) <= 0) {
        $out['dpp'] = round($h['duration'] / ($h['page'] - $h['bounce']));
      }
      $out['span'] = ceil($h['span'] / $h['returning']);
      if ($out['user'] > 0) {
        $out['woman'] = number_format(($h['woman'] / $out['user']) * 100, 1).'%';
      }
      else {
        $out['woman'] = '-';
      }
    }
    elseif ($name == 'medium') {
      $out = array();
      foreach (Tracker::$mediums as $id => $label) {
        if ($h[$id]['metric']['count'] < 1) continue ;
        $out[$id] = number_format(($h[$id]['metric']['count'] / $this->visits) * 100, 1);
      }
    }
    elseif ($name == 'event') {
      $out = array();
      $strings = $this->getStrings(array_keys($h));
      foreach ($h as $gid => $struct) {
        $out[] = array('id' => $gid,
                       'name' => $strings[$gid],
                       'count' => $struct['metric']['count']);
      }
      usort($out, array($this, 'sortName'));
    }
    elseif ($name == 'conversion') {
      $out = array();
      foreach ($h as $model_id => $struct) {
        $out[] = array('id' => $model_id,
                       'name' => l(Model::label($model_id)),
                       'count' => $struct['metric']['count']);
      }
      usort($out, array($this, 'sortName'));
    }
    elseif ($name == 'interaction') {
      $out = array();
      foreach ($h as $model_id => $struct) {
        if (!Model::exists($model_id)) continue ;
        $out[] = array('id' => $model_id,
                       'name' => l(Model::label($model_id)),
                       'count' => (int) $struct['metric']['count']);
      }
      usort($out, array($this, 'sortName'));
    }
    elseif ($name == 'browser') {
      $out = array();
      foreach ($h as $id => $count) {
        $val = number_format(($count / $this->visits) * 100, 1);
        if ($val < 2) continue ;
        $out[self::$browsers[$id]] = $val;
      }
      $out = array_change_key_case($out, CASE_LOWER);
    }
    elseif ($name == 'system') {
      $out = array();
      foreach ($h as $id => $count) {
        $val = number_format(($count / $this->visits) * 100, 1);
        if ($val < 2) continue ;
        $out[self::$systems[$id]] = $val;
      }
      $out = array_change_key_case($out, CASE_LOWER);
    }
    elseif ($name == 'locale') {
      $out = array();
      foreach ($h as $id => $count) {
        $val = number_format(($count / $this->visits) * 100, 1);
        if ($val < 0.2) continue ;
        $out[Locale::toCode($id)] = $val;
      }
      $out = array_change_key_case($out, CASE_LOWER);
    }
    elseif ($name == 'exit' || $name == 'landing') {
      $out = array();
      $strings = $this->getStrings(array_keys($h));
      foreach ($h as $id => $count) {
        $out[$strings[$id]] = number_format(($count / $this->visits) * 100, 1);
      }
      $out = array_slice($out, 0, 5, true);
    }
    elseif ($name == 'share') {
      $out = array();
      $urls = $this->getUrls(array_keys($h));
      foreach ($h as $id => $count) {
        $out[$urls[$id]] = $count;
      }
      $out = array_slice($out, 0, 10, true);
    }
    elseif ($name == 'depth') {
      $out = $h;
      ksort($out);
    }
    elseif ($name == 'country') {
      $out = array();
      $field = BackManager::getLocale() == I18N::LOCALE_FR ? 'name_fr' : 'name_en';
      $countries = $this->dbo->asHash('SELECT id, code, '.$field.' AS name'.
                                      ' FROM '.T('country')
                                      .' WHERE id IN ([countries])',
                                      array('[countries]' => array_keys($h)),
                                      'id');
      foreach ($h as $id => $count) {
        $out[] = array('code' => $countries[$id]['code'],
                       'name' => $countries[$id]['name'],
                       'count' => $count,
                       'rate' => (int) round((($count / $this->visits) * 100), 1));
      }
      $out = array_slice($out, 0, 20, true);
    }
    else {
      $out = $h;
    }
    return $out;
  }

  public function getChart() {

  }

  public function getSteps() {
    if ($this->isDay()) {
      $hours = $this->attributes['hour'];
      foreach (range(0, 23) as $i) {
        $hours[$i] += 0;
      }
      ksort($hours);
      return $hours;
    }
    if ($this->isMonth()) {
      $n = cal_days_in_month(CAL_GREGORIAN,
                             $this->month,
                             $this->year);
      $days = array();
      foreach (range(1, $n) as $i) {
        $days[$i] += 0;
      }
      $rs = $this->dbo->query('SELECT day, metric'
                              .' FROM '.self::TABLE
                              .' WHERE year='.$this->year
                              .  ' AND month='.$this->month
                              .  ' AND day>0');
      foreach ($rs as $row) {
        $metrics = unserialize($row['metric']);
        $days[$row['day']] += $metrics['visit'];
      }
      ksort($days);
      return $days;
    }
    if ($this->isYear()) {
      $months = array();
      foreach (range(1, 12) as $i) {
        $months[$i] += 0;
      }
      $rs = $this->dbo->query('SELECT month, metric'
                              .' FROM '.self::TABLE
                              .' WHERE year='.$this->year
                              .  ' AND month>0'
                              .  ' AND day=0');
      foreach ($rs as $row) {
        $metrics = unserialize($row['metric']);
        $months[$row['month']] += $metrics['visit'];
      }
      ksort($months);
      return $months;
    }
    if ($this->isGlobal()) {
      $years = array();
      list ($min, $max) = $this->dbo->fetchArray('SELECT MIN(year), MAX(year)'
                                                 .' FROM '.self::TABLE
                                                 .' WHERE year>0'
                                                 .  ' AND month=0');
      foreach (range($min, $max) as $i) {
        $years[$i] += 0;
      }
      $rs = $this->dbo->query('SELECT year, metric'
                              .' FROM '.self::TABLE
                              .' WHERE year>0'
                              .  ' AND month=0'
                              .  ' AND day=0');
      foreach ($rs as $row) {
        $metrics = unserialize($row['metric']);
        $years[$row['year']] += $metrics['visit'];
      }
      ksort($years);
      return $years;
    }
  }

  private function getStrings($set) {
    if (empty($set)) return array();
    return $this->dbo->asHash('SELECT id, name'
                              .' FROM '.Tracker::TABLE_EXPRESSION
                              .' WHERE id IN ([set])',
                              array('[set]' => $set));
  }

  private function getUrls($set) {
    if (empty($set)) return array();
    return $this->dbo->asHash('SELECT id, name'
                              .' FROM '.T('url')
                              .' WHERE id IN ([set])',
                              array('[set]' => $set));
  }

  private function getSqlCondition($column='created_on') {
    if ($this->isGlobal()) {
      return '1';
    }
    if ($this->isYear()) {
      return 'YEAR('.$column.')="'.$this->year.'"';
    }
    if ($this->isMonth()) {
      return 'YEAR('.$column.')="'.$this->year.'" AND MONTH('.$column.')="'.$this->month.'"';
    }
    if ($this->isDay()) {
      return $column.'="'.$this->year.'-'.$this->month.'-'.$this->day.'"';
    }
  }

  public function isGlobal() { return $this->year < 1; }
  public function isYear() { return $this->year >= 1 && $this->month < 1; }
  public function isMonth() { return $this->month >= 1 && $this->day < 1; }
  public function isDay() { return $this->day >= 1; }

  public static function getInstance($period) {
    $class = __CLASS__;
    return new $class($period);
  }

  public static $metric_labels = array('page' => 'nb pages vues',
                                       'duration' => 'durée moyenne',
                                       'span' => 'recurrence',
                                       'visit' => 'nbre visites',
                                       'returning' => '% d’habitués',
                                       'person' => '% d’utilisateurs',
                                       'visit_one' => 'tx rebond',
                                       'visit_ten' => '>= 10e visite',
                                       'visit_five' => '>= 5e visite',
                                       'new' => '% nouvelles visites',
                                       'conversion' => 'nbre de conversions',
                                       'robot' => 'nbre de robots',
                                       'search' => 'nbre recherches');

  const MOBILE_REGEX = 'phone|ipad|ipod|avantgo|webos|mobile|android|docomo|netfront|transcoder|symbian|playstation|midp|mini|nokia|blackberry|plucker|j2me|motorola|palm|samsung|htc|sonyericsson|windows ce|xiino';

  public static function ua2hash($user_agent) {
    if (empty($user_agent)) {
      return array('browser' => 0,
                   'system' => 0);
    }
    $browsers = array('fennec' => 2,
                      'firefox|iceweasel|paradiso' => 1,
                      'msie 9' => 14,
                      'msie 8' => 13,
                      'msie 7' => 12,
                      'msie 6' => 11,
                      'msie' => 10,
                      'chrome' => 20,
                      'iphone' => 30,
                      'android' => 51,
                      'iphone|ipod' => 52,
                      'safari|webkit|khtml' => 31,
                      'opera' => 40,
                      self::MOBILE_REGEX => 50);
    $systems = array('windows nt 6\.0' => 2,
                     'windows nt (6|7)' => 1,
                     'windows nt 5\.0' => 6,
                     'windows nt 5' => 3,
                     'windows' => 6,
                     self::MOBILE_REGEX => 150,
                     'mac os|ppc|powerpc|darwin' => 100,
                     'linux|bsd|sunsos|x11' => 200);
    $browser = 0;
    foreach ($browsers as $regex => $id) {
      if (preg_match('/'.$regex.'/', $user_agent)) {
        $browser = $id;
        break;
      }
    }
    $system = 0;
    foreach ($systems as $regex => $id) {
      if (preg_match('/'.$regex.'/', $user_agent)) {
        $system = $id;
        break;
      }
    }
    return array('browser' => $browser,
                 'system' => $system);
  }

  public static $systems = array(0 => 'unknown',
                                 1 => 'win 7',
                                 2 => 'win vista',
                                 3 => 'win xp',
                                 6 => 'win 95|nt|2k',
                                 100 => 'macos',
                                 150 => 'phone',
                                 200 => 'linux');

  public static $browsers = array(0 => 'unknown',
                                  1 => 'firefox',
                                  2 => 'fennec',
                                  10 => 'ie',
                                  11 => 'ie6',
                                  12 => 'ie7',
                                  13 => 'ie8',
                                  14 => 'ie9',
                                  20 => 'chrome',
                                  30 => 'iphone',
                                  31 => 'safari',
                                  40 => 'opera',
                                  50 => 'mobile',
                                  51 => 'android',
                                  52 => 'iphone');
}