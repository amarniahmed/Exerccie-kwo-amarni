<?php

// tracker.internal.addresses 

class ReportBuilder 
{
  const PATH_DEPTH = 8;
  const MAX = 300;
  const BOUNCE_THRESHOLD = 15;

  private $dbo = null;
  private $debug = false;
  private $stack = null;
  private $exclude = null;

  public function __construct() {
    $this->dbo = S('dbo');
    $this->dbo->getTable(Tracker::TABLE_HIT)->optimize();
    $this->stack = array('years' => array(),
                         'months' => array(),
                         'days' => array());

    $exclude = P('tracker.internal.addresses');
    if (!empty($exclude)) {
      if (!is_array($exclude)) {
        $exclude = SH::asSet($exclude);
      }
      foreach ($exclude as &$ip) {
        $ip = IH::ip2num($ip);
      }
      $this->exclude = ' AND ip NOT IN ('.join(',', $exclude).')';
    }
    else {
      $this->exclude = '';
    }

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
    if (count($this->stack['days']) > 31) {
      S('log')->trace('ReportBuilding : start', __METHOD__);
    }
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
    if (count($this->stack['days']) > 31) {
      S('log')->trace('ReportBuilding : end', __METHOD__);
    }
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
               .             ' COUNT(*) AS depth,'
               .             ' TIMESTAMPDIFF(SECOND, MIN(created_at), MAX(created_at)) AS duration,'
               .             ' CRC32(group_concat(DISTINCT action_id)) AS path'
               .        ' FROM '.Tracker::TABLE_HIT
               .    ' GROUP BY visit_id) AS t'
               .  ' ON t.visit_id=v.id'
               . ' SET v.depth=t.depth,'
               .     ' v.duration=t.duration,'
               .     ' v.path=t.path'
               .' WHERE v.created_on="'.$date.'"');
    $this->trace('update');

    $h['metric']['conversion'] = 0;
    Goal::achieve($date);
    $this->trace('goal achieve');
    $rs = $dbo->query('SELECT c.model_id, v.medium, v.source, v.term, v.campaign'
                      .' FROM '.T('conversion').' AS c,'
                      .     ' '.Visit::TABLE.' AS v'
                      .' WHERE c.visit_id=v.id'
                      .  ' AND DATE(c.converted_at)="'.$date.'"'); 
    foreach ($rs as $row) {
      $model_id = $row['model_id'];
      $h['conversion'][$model_id]['metric']['count'] += 1;
      $h['metric']['conversion'] += 1;
      if ($row['medium'] > 0) {
        $h['conversion'][$model_id]['medium'][$row['medium']] += 1;
      }
      if ($row['source'] > 0) {
        $h['conversion'][$model_id]['source'][$row['source']] += 1;
      }
      if ($row['term'] > 0) {
        $h['conversion'][$model_id]['term'][$row['term']] += 1;
      }
      if ($row['campaign'] > 0) {
        $h['conversion'][$model_id]['campaign'][$row['campaign']] += 1;
      }
    }
    $this->trace('conversion count ('.intval($h['metric']['conversion']).')');
        
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
                                                  .' FROM '.Item::TABLE_LOG
                                                  .' WHERE created_on="'.$date.'"'
                                                  .  ' AND model_id=0'
                                                  .  ' AND record_id=0');
    $h['metric']['page'] = $h['metric']['page'] ?: 1;
    $this->trace('pages ('.$h['metric']['page'].')');
    
    $h['metric']['interaction'] = (int) $dbo->fetchValue('SELECT COUNT(*)'
                                                    .' FROM '.Interaction::TABLE
                                                    .' WHERE created_on="'.$date.'"');
    $h['metric']['interaction'] = $h['metric']['interaction'] ?: 1;

    $h['community'] = array('age' => array(),
                            'origin' => array());
    $rs = $dbo->query('SELECT FLOOR(DATEDIFF("'.$date.'", i.birth_date)/365) AS age,'
                      .     ' i.origin_id, u.flags'
                      .' FROM '.Meta::TABLE.' AS md,'
                      .     ' '.T('user').' AS u,'
                      .     ' '.User::TABLE_INFORMATION.' AS i'
                      .' WHERE md.'.Meta::FIELD_MODEL.'='.Model::id('user')
                      .  ' AND md.'.Meta::FIELD_RECORD.'=u.id'
                      .  ' AND TO_DAYS(md.created_at)=TO_DAYS("'.$date.'")'
                      .  ' AND md.'.Meta::FIELD_RECORD.'=i.user_id'); 
    foreach ($rs as $row) {
      $h['metric']['user'] += 1;
      if (($row['flag'] & User::FLAG_WOMAN) > 0) {
        $h['metric']['woman'] += 1;
      }
      if ($row['origin_id'] >= 1) {
        $h['community']['origin'][$row['origin_id']] += 1;
      }
      foreach (Report::$ages as $key => $b) {
        $age = (int) $row['age'];
        if ($age >= $b[0] && $age <= $b[1]) {
          $h['community']['age'][$key] += 1;
        }
      }
    }
    //      S('log')->debug($h['community']);
    //      S('log')->debug($h['metric']);
    
    $rs = $dbo->query('SELECT model_id, record_id, type, COUNT(*) as n'
                      .' FROM '.Interaction::TABLE
                      .' WHERE created_on="'.$date.'"'
                      .' GROUP BY model_id, record_id, type'
                      .' ORDER BY model_id ASC, record_id ASC, type ASC, n DESC');

    $counter = 0;
    foreach ($rs as $row) {
      $model_id = $row['model_id'];
      $record_id = $row['record_id'];
      $type = $row['type'];
      $n = $row['n'];
      //      list ($model_id, $record_id, $type, $n) = $row;
      $h['interaction'][$model_id]['metric']['count'] += $n;
      $h['interaction'][$model_id]['metric'][$type] += $n;
      if (!is_array($h['interaction'][$model_id][$type]) || 
          count($h['interaction'][$model_id][$type]) < (self::MAX)) {
        $h['interaction'][$model_id][$type][$record_id] = $n;
      }
    }
    $this->trace('interactions');

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
    
    $n = $dbo->fetchValue('SELECT COUNT(DISTINCT visitor_id)'
                          .' FROM '.Visit::TABLE
                          .' WHERE created_on="'.$date.'"'.$this->exclude); 
    $h['metric']['visitor'] = (int) $n;

    $rs = $dbo->query('SELECT *'
                      .' FROM '.Visit::TABLE
                      .' WHERE created_on="'.$date.'"'.$this->exclude);
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

      if ($visit->browser > 0) $h['browser'][$visit->browser] += 1;
      if ($visit->country > 0) $h['country'][$visit->country] += 1;
      if ($visit->locale > 0)  $h['locale'][$visit->locale] += 1; 
      if ($visit->system > 0)  $h['system'][$visit->system] += 1;

      if ($hour < 5 || $hour >= 20) $h['metric']['night'] += 1;     
      if ($visit->user_id > 0)      $h['metric']['person'] += 1;
 
      if ($visit->depth <= 1) {
        $h['metric']['bounce'] += 1;
      }

      if ($visit->depth <= 10) {
        $n = $visit->depth > 0 ? $visit->depth : 1;
        $h['depth'][$n] += 1;
      }
      elseif ($visit->depth >= 100) {
        $h['depth'][100] += 1;
      }
      else {
        $h['depth'][$visit->depth - ($visit->depth % 10)] += 1;
      }
      
      if ($visit->number <= 1) {
        $h['metric']['new'] += 1;
      }
      else {
        $h['metric']['returning'] += 1;
      }

      $h['metric']['duration'] += $visit->duration;
      $h['metric']['span']     += $visit->span;
      
      if ($visit->medium > 0) {
        $h['medium'][$visit->medium]['metric']['count'] += 1;
        if ($visit->campaign > 0) {
          $h['campaign'][$visit->campaign] += 1;
          $h['medium'][$visit->medium]['campaign'][$visit->campaign] += 1;
        }
        if ($visit->source > 0) {
          $h['source'][$visit->source] += 1;
          $h['medium'][$visit->medium]['source'][$visit->source] += 1;
        }
        if ($visit->term > 0) {
          $h['term'][$visit->term] += 1;
          $h['medium'][$visit->medium]['term'][$visit->term] += 1;
        }
        if ($visit->medium === Tracker::MEDIUM_SOCIAL && $visit->url_id > 0) {
          $h['share'][$visit->url_id] += 1;
        }
      } 

      if ($visit->depth >= 1) {
        $events = $dbo->query('SELECT created_at, category_id, action_id'
                              .' FROM '.Tracker::TABLE_HIT
                              .' WHERE visit_id='.$visit->id);
        $path = array();
        $n = count($events) - 1;
        foreach ($events as $i => $event) {
          $action_id = $event['action_id'];
          $category_id = $event['category_id'];
          $h['event'][$category_id]['action'][$action_id] += 1;
          $h['event'][$category_id]['metric']['count'] += 1;
          //          if ($i == 0 && $category_id != $this->category_auth) {
          if ($i == 0) {
            $h['landing'][$action_id] += 1;
          }
          if ($n > 1 && $i == $n) {
            $h['exit'][$action_id] += 1;
          }
          if (count($path) <= self::PATH_DEPTH && !in_array($action_id, $path)) {
            $path[] = $action_id;
          }
          if (count($path) == self::PATH_DEPTH) {
            $h['path'][join(',', $path)] += 1;
          }
        }
      }
    }

    $dpp = $h['metric']['duration'] / ($h['metric']['page'] - $h['metric']['bounce']);
    $h['metric']['duration'] += $dpp * $h['metric']['visit']; 

    $time = mktime(0, 0, 0, $month, $day, $year);

    //    $week_day = date('N', $time);
    
    $row  = $this->merge(array($h));
    $row += array('year' => $year,
                  'month' => $month,
                  'week' => date('W', $time),
                  'day' => $day,
                  'date' => $date);

    S('app')->hook('onReportBuild', array('row' => &$row));

    $dbo->exec('DELETE FROM '.Report::TABLE
               .    ' WHERE year='.$year
               .      ' AND month='.$month
               .      ' AND day='.$day);
    $this->insert($row);
    $this->trace('day row insert');
    
  }

  private function buildMonth($month) { 
    list ($year, $month) = explode('-', $month);
    $dbo = $this->dbo;
    $dbo->exec('DELETE FROM '.Report::TABLE.' WHERE year='.$year.' AND month='.$month.' AND day=0');
    $rs = $dbo->query('SELECT * FROM '.Report::TABLE.' WHERE year='.$year.' AND month='.$month.' AND day>0');
    $row  = $this->merge($rs);
    $row += array('year' => $year,
                  'month' => $month,
                  'day' => 0);
    $days = cal_days_in_month(CAL_GREGORIAN, $month, $year);
    $n = $dbo->fetchValue('SELECT COUNT(DISTINCT visitor_id)'
                          .' FROM '.Visit::TABLE
                          .' WHERE created_on>="'.$year.'-'.intval($month).'-01"'
                          .  ' AND created_on<="'.$year.'-'.intval($month).'-'.$days.'"'.$this->exclude);
    $row['metric']['visitor'] = (int) $n;
    $this->insert($row);
  }
  
  private function buildYear($year) {
    $dbo = $this->dbo;
    $dbo->exec('DELETE FROM '.Report::TABLE.' WHERE year='.$year.' AND month=0 AND day=0');
    $rs = $dbo->query('SELECT * FROM '.Report::TABLE.' WHERE year='.$year.' AND month>0 AND day=0');
    $row  = $this->merge($rs);
    $row += array('year' => $year,
                  'month' => 0,
                  'day' => 0);
    $n = $dbo->fetchValue('SELECT COUNT(DISTINCT visitor_id)'
                          .' FROM '.Visit::TABLE
                          .' WHERE created_on>="'.$year.'-01-01"'
                          .  ' AND created_on<="'.$year.'-12-31"'.$this->exclude);
    $row['metric']['visitor'] = (int) $n;
    $this->insert($row);
  }
  
  private function buildGlobal() {
    $dbo = $this->dbo;
    $dbo->exec('DELETE FROM '.Report::TABLE.' WHERE year=0 AND month=0 AND day=0');
    $rs = $dbo->query('SELECT * FROM '.Report::TABLE.' WHERE year>0 AND month=0 AND day=0');
    $row  = $this->merge($rs);
    $row += array('year' => 0,
                  'month' => 0,
                  'day' => 0);
    $n = $dbo->fetchValue('SELECT COUNT(DISTINCT visitor_id)'
                          .' FROM '.Tracker::TABLE_VISITOR
                          .' WHERE 1'.$this->exclude);
    $row['metric']['visitor'] = (int) $n;
    $this->insert($row);
  }

  private function insert($row) {
    foreach ($row as &$value) {
      if (!is_array($value)) continue ;
      $value = serialize($value);
    }
    $this->dbo->insert(Report::TABLE, $row);
  }

  private function merge($rs) {
    $record = array();

    foreach ($rs as $row) {
      foreach ($row as $dimension => $h) {
        if (!array_key_exists($dimension, Report::$dimensions)) continue ;
        if (empty($h)) $h = array();
        $h = is_array($h) ? $h : unserialize($h);
        if ($h === false) continue ;
        $record[$dimension] = (array) $record[$dimension];
        if (Report::$dimensions[$dimension] == 3) {
          foreach ($h as $gid => $struct) {
            //            if ($struct['count'] < 1) continue ;
            $record[$dimension][$gid] = (array) $record[$dimension][$gid];
            //            $record[$dimension][$gid]['count'] += $struct['count'];
            foreach ($struct as $key => $value) {
              //              if ($key == 'count') continue ;
              $record[$dimension][$gid][$key] = (array) $record[$dimension][$gid][$key];
              $struct[$key] = (array) $struct[$key];
              foreach ($struct[$key] as $id => $n) {
                $record[$dimension][$gid][$key][$id] += $n;
              }
            }
          }
        }
        elseif (Report::$dimensions[$dimension] == 2) {
          foreach ($h as $type => $values) {
            $record[$dimension][$type] = (array) $record[$dimension][$type];
            foreach ($values as $key => $value) {
              $record[$dimension][$type][$key] += $value;
            }
          }
        }
        else {
          foreach ($h as $key => $value) {
            $record[$dimension][$key] += $value;
          }
        }
      }
    }

    foreach ($record as $dimension => &$h) {
      if (Report::$dimensions[$dimension] == 3) {
        foreach ($h as $gid => &$struct) {
          foreach ($struct as $key => $value) {
            //            if ($key == 'count') continue ;
            arsort($struct[$key], SORT_NUMERIC); 
            if ($key != 'metric') {
              $struct[$key] = array_slice($struct[$key], 0, (self::MAX / 2), true);
            }
          }
        }
      }
      elseif (Report::$dimensions[$dimension] == 2) {
        
      }
      else {
        arsort($h, SORT_NUMERIC); 
        $h = array_slice($h, 0, self::MAX, true);
      }
      //$h = serialize($h);
    }

    return $record;
  }
  
}
