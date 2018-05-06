<?php

  // utiliser le terme : metric
  // une metric est dans une dimension

class ReportBuilder 
{
  const PATH_LENGTH = 8;
  const MAX = 200;

  private $dbo = null;
  private $debug = false;
  private $stack = null;

  public function __construct() {
    $this->dbo = S('dbo');
    $this->dbo->getTable(Tracker::TABLE_EVENT)->optimize();
    $this->stack = array('years' => array(),
                         'months' => array(),
                         'days' => array());
  }
  
  public function enqueue($date) {
    if (is_array($date)) {
      foreach ($date as $d) {
        $this->enqueue($d);
      }
      return ;
    }
    $date = DH::toDb($date); 
    if (DH::isNull($date)) {
      throw new Exception('invalid date ['.$date.']');
    }
    list ($year, $month, $day) = explode('-', $date);
    $year  = (int) $year;
    $month = (int) $month;
    $day   = (int) $day;
    $this->stack['years'][$year]++;
    $this->stack['months'][$year.'-'.$month]++;
    $this->stack['days'][$year.'-'.$month.'-'.$day]++;
  }

  public function process() {
    foreach ($this->stack['days'] as $day => $n) {
      $this->trace('build day ['.$day.']');
      $this->buildDay($day);
    }
    foreach ($this->stack['months'] as $month => $n) {
      $this->trace('build month ['.$month.']');
      $this->buildMonth($month);
    }
    foreach ($this->stack['years'] as $year => $n) {
      $this->trace('build year ['.$year.']');
      $this->buildYear($year);
    }
    $this->trace('build global');
    $this->buildGlobal();
    $this->trace('end');
  }

  public function trace($msg) {
    if ($this->debug != true) return ;
    S('log')->trace($msg);
  }
  
  private function buildDay($date) { 

    $dbo = $this->dbo;

    list ($year, $month, $day) = explode('-', $date);

    $h = array();

    foreach (Report::$dimensions as $dimension => $depth) {
      $h[$dimension] = array();
    }
   
    $dbo->exec('UPDATE '.Visit::TABLE.' AS v'
               .' JOIN (SELECT visit_id,'
               .             ' COUNT(*) AS length,'
               .             ' TIMESTAMPDIFF(MINUTE, MIN(created_at), MAX(created_at)) AS duration,'
               .             ' CRC32(group_concat(DISTINCT action_id)) AS path'
               .        ' FROM '.Tracker::TABLE_EVENT
               .    ' GROUP BY visit_id) AS t'
               .  ' ON t.visit_id=v.id'
               . ' SET v.length=t.length,'
               .     ' v.duration=t.duration,'
               .     ' v.path=t.path'
               .' WHERE v.created_on="'.$date.'"');
    $this->trace('update');

    $h['metric']['conversion'] = 0;
    if (Extension::exists('performance')) {
      $rs = $dbo->query('SELECT model_id, COUNT(*) AS n'
                        .' FROM '.Conversion::TABLE
                        .' WHERE DATE(created_at)="'.$date.'"'
                        .' GROUP BY model_id');
      foreach ($rs as $row) {
        $h['conversion'][$row['model_id']] = $row['n'];
        $h['metric']['conversion'] += $row['n'];
      }
      $this->trace('conversion count ('.intval($h['metric']['conversion']).')');
      Goal::achieve($date);
      $this->trace('goal achieve');
    }
    
    $visit_count = (int) $dbo->fetchValue('SELECT COUNT(*)'
                                          .' FROM '.Visit::TABLE
                                          .' WHERE created_on="'.$date.'"');
    
    if ($visit_count < 1) {
      S('log')->trace('no stat ('.$date.')', __METHOD__);
      return ;
    }
    
    $h['metric']['robot'] = (int) $dbo->fetchValue('SELECT COUNT(*)'
                                                   .' FROM '.Tracker::TABLE_ROBOT
                                                   .' WHERE DATE(created_at)="'.$date.'"');
    $this->trace('robots ('.$h['metric']['robot'].')');

    $h['metric']['page'] = (int) $dbo->fetchValue('SELECT counter'
                                                  .' FROM '.Tracker::TABLE_PAGE
                                                  .' WHERE `day`="'.$date.'"');
    $this->trace('pages ('.$h['metric']['page'].')');

    $rs = $dbo->query('SELECT model_id, record_id, COUNT(record_id) AS n'
                      .' FROM tkr_item'
                      .' WHERE creation_date="'.$date.'"'
                      .' GROUP BY model_id, record_id');
    $counter = 0;
    foreach ($rs as $row) {
      list ($model_id, $record_id, $n) = $row;
      $h['item'][$model_id]['count'] += $n;
      $h['item'][$model_id]['elts'][$record_id] = $n;
      $counter += $n;
    }
    $this->trace('items ('.$counter.')');

    $rs = $dbo->query('SELECT query_id, COUNT(*) AS n'
                      .' FROM '.Tracker::TABLE_SEARCH
                      .' WHERE created_on="'.$date.'"'
                      .' GROUP BY query_id');
    $counter = 0;
    foreach ($rs as $row) {
      list ($query_id, $n) = $row;
      $h['search'][$query_id] = $n;
      $counter += $n;
    }
    $h['metric']['search'] = $counter;
    $this->trace('searches ('.$counter.')');
    
    $rs = $dbo->query('SELECT *'
                      .' FROM '.Visit::TABLE
                      .' WHERE created_on="'.$date.'"');
    $this->trace('visit ('.count($rs).')');
    $j = 1;
    while ($visit = $rs->fetchObject()) {
      
      if (($j % 5000) === 0) {
        $percent = round(($j / $visit_count) * 100, 1);
        $this->log->trace('['.$date.'] '.$percent.'% (total='.$visit_count.')');
        $j++;
      }
      
      $h['metric']['visit'] += 1;
      
      $hour = (int) substr($visit->created_at, 0, 2);

      $h['hour'][$hour] += 1;

      if ($visit->campaign > 0) $h['campaign'][$visit->campaign] += 1;
      if ($visit->country > 0)  $h['country'][$visit->country] += 1;
      if ($visit->browser > 0)  $h['browser'][$visit->browser] += 1;
      if ($visit->host > 0)     $h['host'][$visit->host] += 1;
      if ($visit->locale > 0)   $h['locale'][$visit->locale] += 1;
      if ($visit->medium > 0)   $h['medium'][$visit->medium] += 1;
      if ($visit->source > 0)   $h['source'][$visit->source] += 1;
      if ($visit->system > 0)   $h['system'][$visit->system] += 1;
      if ($visit->term > 0)     $h['term'][$visit->term] += 1;
 
      if ($hour < 5 || $hour >= 20)  $h['metric']['night'] += 1;     
      if ($visit->user_id > 0)       $h['metric']['user'] += 1;
 
      if ($visit->length <= 1)       $h['metric']['bounce'] += 1;
      elseif ($visit->length >= 10)  $h['metric']['visit_ten'] += 1;
      elseif ($visit->length >= 5)   $h['metric']['visit_five'] += 1;
      
      if ($visit->number == 1) {
        $h['metric']['new'] += 1;
      }
      else {
        $h['metric']['returning'] += 1;
      }

      $h['metric']['duration'] += $visit->duration;
      $h['metric']['span']     += $visit->span;
      
      if ($visit->length >= 1) {
        $events = $dbo->query('SELECT created_at, category_id, action_id'
                              .' FROM '.Tracker::TABLE_EVENT
                              .' WHERE visit_id='.$visit->id);
        $path = array();
        $n = count($events) - 1;
        foreach ($events as $i => $event) {
          $action_id = $event['action_id'];
          $h['event'][$event['category_id']]['elts'][$action_id] += 1;
          $h['event'][$event['category_id']]['count'] += 1;
          if ($i == 0) {
            $h['landing'][$action_id] += 1;
          }
          if ($n > 1 && $i == $n) {
            $h['exit'][$action_id] += 1;
          }
          if (count($path) <= self::PATH_LENGTH && !in_array($action_id, $path)) {
            $path[] = $action_id;
          }
          if (count($path) == self::PATH_LENGTH) {
            $h['path'][join(',', $path)] += 1;
          }
        }
      }
      
    }

    $time = mktime(0, 0, 0, $month, $day, $year);

    //    $week_day = date('N', $time);
    
    $record  = $this->merge(array($h));
    $record += array('year' => $year,
                     'month' => $month,
                     'week' => date('W', $time),
                     'day' => $day,
                     'date' => $date);
    
    $dbo->exec('DELETE FROM '.Report::TABLE.' WHERE year='.$year.' AND month='.$month.' AND day='.$day);
    $dbo->insert(Report::TABLE, $record);
    $this->trace('day record insert');

  }

  private function buildMonth($month) { 
    list ($year, $month) = explode('-', $month);
    $this->dbo->exec('DELETE FROM '.Report::TABLE.' WHERE year='.$year.' AND month='.$month.' AND day=0');
    $rs = $this->dbo->query('SELECT * FROM '.Report::TABLE.' WHERE year='.$year.' AND month='.$month.' AND day>0');
    $record = $this->merge($rs);
    $record += array('year' => $year,
                     'month' => $month,
                     'day' => 0);
    $this->dbo->insert(Report::TABLE, $record);
  }

  private function buildYear($year) {
    $this->dbo->exec('DELETE FROM '.Report::TABLE.' WHERE year='.$year.' AND month=0 AND day=0');
    $rs = $this->dbo->query('SELECT * FROM '.Report::TABLE.' WHERE year='.$year.' AND month>0 AND day=0');
    $record = $this->merge($rs);
    $record += array('year' => $year,
                     'month' => 0,
                     'day' => 0);
    $this->dbo->insert(Report::TABLE, $record);
  }

  private function buildGlobal() {
    $this->dbo->exec('DELETE FROM '.Report::TABLE.' WHERE year=0 AND month=0 AND day=0');
    $rs = $this->dbo->query('SELECT * FROM '.Report::TABLE.' WHERE year>0 AND month=0 AND day=0');
    $record = $this->merge($rs);
    $record += array('year' => 0,
                     'month' => 0,
                     'day' => 0);
    $this->dbo->insert(Report::TABLE, $record);
  }

  private function merge($rs) {
    $record = array();
    foreach ($rs as $row) {
      foreach ($row as $dimension => $hash) {
        if (!array_key_exists($dimension, Report::$dimensions)) continue ;
        if (empty($hash)) $hash = array();
        $hash = is_array($hash) ? $hash : unserialize($hash);
        if ($hash === false) continue ;
        $record[$dimension] = (array) $record[$dimension];
        if (Report::$dimensions[$dimension] == 2) {
          foreach ($hash as $gid => $elts) {
            if (!is_array($elts['elts'])) continue ;
            $record[$dimension][$gid] = (array) $record[$dimension][$gid];
            $record[$dimension][$gid]['elts'] = (array) $record[$dimension][$gid]['elts'];
            $record[$dimension][$gid]['count'] += $elts['count'];
            foreach ($elts['elts'] as $id => $n) {
              $record[$dimension][$gid]['elts'][$id] += $n;
            }
          }
        }
        else {
          foreach ($hash as $key => $value) {
            $record[$dimension][$key] += $value;
          }
        }
      }
    }
    foreach ($record as $dimension => &$hash) {
      if (Report::$dimensions[$dimension] == 2) {
        foreach ($hash as $gid => &$elts) {
          arsort($elts['elts'], SORT_NUMERIC); 
          $elts['elts'] = array_slice($elts['elts'], 0, (self::MAX / 2), true);
        }
      }
      else {
        arsort($hash, SORT_NUMERIC); 
        $hash = array_slice($hash, 0, self::MAX, true);
      }
      $hash = serialize($hash);
    }
    return $record;
  }
  
}

  /*  public function start() {
    $dbo = $this->dbo;
    $log = $this->log;

   if ($this->mode === 'dev') {
      if (Extension::exists('performance')) {
        $dbo->exec('UPDATE '.Visit::TABLE.' AS v'
                   .' JOIN (SELECT visit_id,'
                   .             ' COUNT(*) AS conversions'
                   .        ' FROM '.Conversion::TABLE
                   .    ' GROUP BY visit_id) AS c'
                   .  ' ON c.visit_id=v.id'
                   .' SET v.conversions=c.conversions'); 
      }
   }
    
    $this->debug('PROCESS LOGS');
    
    $dbo->getTable(Tracker::TABLE_EVENT)->optimize();

    list ($min, $max) = $dbo->fetchArray('SELECT MIN(visit_id), MAX(visit_id)'
                                         .' FROM '.Tracker::TABLE_EVENT
                                         .' WHERE DATE(created_at)="'.$date.'"');
    
    $this->debug('min='.$min.', max='.$max);
    
    if ($min < 1 || $max < 1) { 
      S('log')->trace('no stat today (1)', __METHOD__);
      return ;
    }

    $this->debug('UPDATE VISITS');

    $dbo->exec('UPDATE '.Visit::TABLE.' AS v'
               .' JOIN (SELECT visit_id,'
               .             ' COUNT(*) AS length,'
               .             ' TIMESTAMPDIFF(MINUTE, MIN(created_at), MAX(created_at)) AS duration,'
               .             ' CRC32(group_concat(DISTINCT action_id)) AS path'
               .        ' FROM '.Tracker::TABLE_EVENT
               .      ' GROUP BY visit_id) AS t'
               .  ' ON t.visit_id=v.id'
               . ' SET v.length=t.length,'
               .     ' v.duration=t.duration,'
               .     ' v.path=t.path'
               .' WHERE v.id BETWEEN '.$min.' AND '.$max);

    $this->debug('FIND DATES');

    $rs = $dbo->query('SELECT DISTINCT(DATE_FORMAT(created_at, "%Y-%m-%v-%d")) AS dateweek'
                      .' FROM '.Tracker::TABLE_EVENT
                      .' WHERE DATE(created_at)="'.$date.'"'
                      .' ORDER BY dateweek');

    if ($rs === false || $rs->numRows() < 1) { 
      S('log')->trace('no stat today (2)', __METHOD__);
      return ; 
    }
    
    $builds = array();
    while ($obj = $rs->fetchObject()) {
      list($year, $month, $week, $day) = explode('-', $obj->dateweek);
      $year = (int) $year;
      $month = (int) $month;
      $week = (int) $week;
      $day = (int) $day;
      if ($year < 1 || $month < 1 || $week < 1 || $day < 1) continue ;
      $builds['day'][$year.'-'.$month.'-'.$week.'-'.$day] = 1;
      $builds['week'][$year.'-'.$week] = 1;
      $builds['month'][$year.'-'.$month] = 1;
      $builds['year'][$year] = 1;
    }

    if (count($builds) < 1) { throw new Exception('no stat today (2)'); }

    $this->debug($builds);
    
    foreach ($builds['day'] as $key => $null) {

      foreach (self::$cols as $col) {
        ${$col.'_arr'} = array();
      }

      list ($year, $month, $week, $day) = explode('-', $key);

      $date = $year.'-'.$month.'-'.$day;
      
      if (Extension::exists('performance')) {
        Goal::achieve($date);
      }
        
      $week_day = date('N', mktime(0, 0, 0, $month, $day, $year)); 
      $nb_day_in_month = date('d', mktime(0, 0, 0, $month + 1, 0, $year));
      
      $hour_arr = array_fill(0, 24, array());
      
      $nb_visit = $dbo->fetchValue('SELECT COUNT(*)'
                                   .' FROM '.Visit::TABLE
                                   .' WHERE created_on="'.$date.'"');
      $this->debug('NB VISIT ('.$year.'-'.$month.'-'.$day.') = '.$nb_visit);
      
       
      $qty_arr['robot'] = $dbo->fetchValue('SELECT COUNT(*) FROM '.Tracker::TABLE_ROBOT
                                           .' WHERE DATE(created_at)="'.$date.'"');
      
      $qty_arr['page'] = (int) $dbo->fetchValue('SELECT counter'
                                                .' FROM tkr_page'
                                                .' WHERE `day`="'.$date.'"');
      $rs = $dbo->query('SELECT *'
                       .' FROM '.Visit::TABLE
                       .' WHERE created_on="'.$date.'"');
      $j = 0;
      while ($visit = $rs->fetchObject()) {

	if ($nb_visit > 5000 && (++$j % 5000) === 0) {
	  $percent = round(($j / $nb_visit) * 100, 1);
	  $this->log->trace('['.$month.'/'.$day.'] '.$percent.'% (total='.$nb_visit.')');
	}

	$hour = (int) substr($visit->created_at, 0, 2);

	$qty_arr['visit'] += 1;
	
	if ($visit->country >= 1) { 
          $geo_arr['country'][$visit->country] += 1; 
        }

        $host_arr[$visit->host]++;

        if ($visit->user_id >= 1) { $qty_arr['user'] += 1; }

	$hour_arr[$hour]['qty']['visit'] += 1;

	if ($visit->number == 1) {
	  $qty_arr['visitor'] += 1;
	  $hour_arr[$hour]['qty']['visitor'] += 1;
	}
	else {
          $qty_arr['custom'] += 1;
        }

	if ($hour < 5 || $hour >= 19) { $qty_arr['night'] += 1; }
	if (!empty($visit->browser)) { $sys_arr['browser'][$visit->browser] += 1; }
	if (!empty($visit->system)) { $sys_arr['system'][$visit->system] += 1; }

	if (!empty($visit->medium)) {
	  $referer_arr[$visit->medium]['qty']++;
	  if (!empty($visit->source)) {
            $referer_arr[$visit->medium]['source'][$visit->source]++;
          }
          if (!empty($visit->term)) {
            $referer_arr[$visit->medium]['term'][$visit->term]++;
          }
	}
           
	if ($visit->length <= 1) { $qty_arr['visit_one'] += 1; }
	elseif ($visit->length >= 10) { $qty_arr['visit_ten'] += 1; }
	elseif ($visit->length >= 5) { $qty_arr['visit_five'] += 1; }

        $qty_arr['duration'] += $visit->duration;

        $qty_arr['freq'] += $visit->span;
        
        if ($visit->length >= 1) {
          $events = $dbo->query('SELECT created_at, category_id, action_id'
                                .' FROM '.Tracker::TABLE_EVENT
                                .' WHERE visit_id='.intval($visit->id));
          $in = $out = $last = '';
          $path = array();
          foreach ($events as $i => $event) {
            if ($i === 0) $in = $event['action_id'];
            if (count($path) <= self::PATH_LENGTH && !in_array($event['action_id'], $path)) {
              $path[] = $event['action_id'];
            }
            $out = $event['action_id'];
            $chapter_arr[$event['category_id']]['title_arr'][$event['action_id']] += 1;
	    $chapter_arr[$event['category_id']]['qty'] += 1;
            $last = $event['action_id']; 
          }
          if ($i >= 1) {
            $navig_arr['in'][$in] += 1;
          }
          if ($i >= 5) {
            $navig_arr['out'][$out] += 1;
            if (count($path) >= self::PATH_LENGTH) {
              $navig_arr['path'][join(',', $path)] += 1;
            }
          }
        }

	unset($visit);
      }

      unset($visit_arr);
      
      $qty_arr['we'] = $week_day >= 6 ? $qty_arr['visit'] : 0;
      
      $day_arr = array_fill(1, $nb_day_in_month, array());
      $week_arr = array_fill(1, 7, array());
      $month_arr = array_fill(1, 12, array());
      $year_arr[$year] = array();

      $arr = array('visit' => $qty_arr['visit'],
                   'visitor' => $qty_arr['visitor']);

      $day_arr[$day]['qty'] = $week_arr[$week_day]['qty'] = $month_arr[$month]['qty'] = $year_arr[$year]['qty'] = $arr;
      
      foreach ($chapter_arr as $chapter => $arr) {
	$day_arr[$day]['chapter'][$chapter] = $week_arr[$week_day]['chapter'][$chapter] = $month_arr[$month]['chapter'][$chapter] = $year_arr[$year]['chapter'][$chapter] = $arr['qty'];
      }
     
      $hash['year'] = $year;
      $hash['month'] = $month;
      $hash['week'] = $week;
      $hash['day'] = $day;
      $hash['week_day'] = $week_day;
      foreach (self::$cols as $col) {
        $column = $col.'_arr';
        $hash[$column] = self::convert(${$column}, $this->max);
      }
      
      //      $this->debug($qty_arr);

      $ret = $dbo->insert(Report::TABLE, $hash);
      if ($ret === false) {
        $this->log->fatal('insertion failure ['.$year.'/'.$month.'/'.$day.']');
      }
    }
    
    $this->debug("PROCESS DAY ------------------------");
    foreach ($builds['day'] as $k => $v) {
      list($year, $month, $week, $day) = explode('-', $k);
      $period = array_fill(0, 24, array());
      $sql = 'SELECT * FROM '.Report::TABLE.' WHERE year='.$year.' AND month='.$month.' AND week='.$week.' AND day='.$day.' ORDER BY year, month, day';
      $this->processStat('day', $year, $month, $week, $day, $sql, $period);
    }
    
    $this->debug("PROCESS WEEK ----------------------");
    foreach ($builds['week'] as $k => $v) {
      list($year, $week) = explode('-', $k);
      $period = array_fill(1, 7, array());
      $sql = 'SELECT * FROM '.Report::TABLE.' WHERE year='.$year.' AND month<>0 AND week='.$week.' AND day<>0 ORDER BY year, week, day';
      $this->processStat('week', $year, 0, $week, 0, $sql, $period);
    }
    
    $this->debug("PROCESS MONTH ----------------------");
    foreach ($builds['month'] as $k => $v) {
      list($year, $month) = explode('-', $k);
      $period = array_fill(1, (int) date('d', mktime(0, 0, 0, ($month + 1), 0, $year)), array());
      $sql = 'SELECT * FROM '.Report::TABLE.' WHERE year='.$year.' AND month='.$month.' AND week<>0 AND day<>0 ORDER BY year, month, day';
      $this->processStat('month', $year, $month, 0, 0, $sql, $period);
    }
    
    $this->debug("PROCESS YEAR -----------------------");
    foreach ($builds['year'] as $year => $v) {
      $period = array_fill(1, 12, array());
      $sql = 'SELECT * FROM '.Report::TABLE.' WHERE year='.$year.' AND month<>0 AND week=0 AND day=0 ORDER BY year, month';
      $this->processStat('year', $year, 0, 0, 0, $sql, $period);
    }
    
    $this->debug("PROCESS GLOBAL -----------------------");
    if (count($builds['day']) > 0) {
      $sql = 'SELECT * FROM '.Report::TABLE.' WHERE year<>0 AND month=0 AND week=0 AND day=0 ORDER BY year';
      $this->processStat('global', 0, 0, 0, 0, $sql, array());
    }
    
    if ($nb_visit >= 5000) {
      $this->log->trace('finished');
    }

  }
  
  protected function processStat($type, $year, $month, $week, $day,
                                 $sql, $period) {

    foreach (self::$cols as $col) {
      $builds[$col.'_arr'] = array();
    }

    $rs = $this->dbo->query($sql);

    if ($rs === false || $rs->numRows() < 1) {
      return ;
    }

    while ($obj = $rs->fetchObject()) {
      foreach ($builds as $name => $arr) {
        $builds[$name] = AH::merge($builds[$name], $obj->{$name});
      }
    }
    
    $this->dbo->exec('DELETE FROM tkr_cache'
                     .' WHERE year='.$year.' AND month='.$month
                     .  ' AND week='.$week.' AND day='.$day);

    $hash['year'] = $year;
    $hash['month'] = $month;
    $hash['week'] = $week;
    $hash['day'] = $day;
    $hash['equiv_date'] = $year.'-'.$month.'-'.$day;

    foreach ($builds as $name => $arr) {
      $hash[$name] = self::convert($arr,$this->max);
    }

    $ret = $this->dbo->insert(Report::TABLE, $hash);
    
    if ($ret === false) {
      $this->log->fatal('process stat ['.$type.'-'.$year.'-'.$month.'-'.$day.']');
    }
    
    //    $this->debug($year.'-'.$month.'-'.$week.'-'.$day);
    //    $this->debug($builds['qty_arr']);
  }

  public static function convert($arr, $max=50) {
    $do_sort = true;
    $arr = empty($arr) ? array() : (array) $arr;
    if (count($arr) <= 0) return serialize($arr);
    $ret_arr = array();
    foreach ($arr as $k0 => $v0) {
      if (!is_array($v0)) continue ;
      foreach ($v0 as $k1 => $v1) {
        if (!is_array($v0[$k1])) continue ;
        arsort($arr[$k0][$k1], SORT_NUMERIC); 
        $arr[$k0][$k1] = array_slice($arr[$k0][$k1], 0, $max, true);
        $do_sort = false;        
      }
      if ($do_sort === true) {
        arsort($arr[$k0], SORT_NUMERIC); 
        $arr[$k0] = array_slice($arr[$k0], 0, $max, true);
      }
    }
    ksort($arr);
    return serialize($arr);
  }

  protected function debug($val) {
    if ($this->debug === false) return false;
    S('log')->debug($val);
    } */