<?php

if ($req->hasAttribute('datetime', true) && !DH::isNull($req->datetime)) {
  list($date, $time) = explode(' ', $req->datetime);
  list($day, $month, $year) = explode('/', $date);
  list($hour, $minute) = explode(':', $time);
  $ref = getdate(mktime((int)$hour, $minute, 0, $month, $day, $year));
  $minute = $ref['minutes'];
  $hour   = $ref['hours'];
  $day    = $ref['mday'];
  $month  = $ref['mon'];
  $year   = $ref['year'];
}
else {
  $minute = date('i');
  $hour = date('H');
  $day = date('d');
  $month = date('m');
  $year = date('Y');
}
$month = str_pad($month, 2, '0', STR_PAD_LEFT);
$day = str_pad($day, 2, '0', STR_PAD_LEFT);
$hour = str_pad($hour, 2, '0', STR_PAD_LEFT);
$minute = str_pad($minute, 2, '0', STR_PAD_LEFT);

if ($req->hasAttribute('from', true)) {
  list($d, $m, $y) = explode('/', $req->from);
  $from = mktime(0, 0, 0, $m, $d, $y);
}
else {
  $from = mktime(0, 0, 0, $month, 1, $year);
}

if ($req->hasAttribute('to', true)) {
  list($d, $m, $y) = explode('/', $req->to);
  $to = mktime(0, 0, 0, $m, $d, $y);
}
else {
  $to = mktime(0, 0, 0, $month, date('t', mktime(0, 0, 0, $month, 1, $year)), $year);
}

$default_type = $req->default_type == 'deny' ? 'deny' : 'allow';
$empty_button = $req->empty_button == 'false' ? false : true;
$has_time = $req->has_time == 'true' ? true : false;


if ($req->hasAttribute('periods', true)) {
  $periods = json_decode($req->periods);
}
if (!is_array($periods)) {
  $periods = array();
}

foreach ($periods as $k => &$period) {
  if (!empty($period->from)) {
    list($d, $m, $y) = explode('/', $period->from);
    $period->from = mktime(0, 0, 0, $m, $d, $y);
  }
  else {
    $period->from = $from;
  }
  if (!empty($period->to)) {
    list($d, $m, $y) = explode('/', $period->to);
    $period->to = mktime(0, 0, 0, $m, $d, $y);
  }
  else {
    $period->to = $to;
  }
  $period->type = $period->type == 'deny' ? 'deny' : 'allow';
  if ($period->every == 'weekend') {
    if (date('N', mktime(0,0,0,$month, 1, $year)) == 7) {
      $tmp = new stdClass;
      $tmp->to = $tmp->from = mktime(0, 0, 0, $month, 1, $year);
      $tmp->type = $period->type;
      $tmp->className = $period->className;
      $periods[] = $tmp;
    }
    $nb_days = date('t', mktime(0, 0, 0, $month, 1, $year));
    $d = 1;
    do {
      $ref = strtotime($year."-".$month."-".$d);
      $saturday = strtotime("saturday", $ref);
      if (date('m', $saturday) != (int)$month) {
        break;
      }
      $sunday = strtotime("sunday", $ref);
      $tmp = new stdClass;
      $tmp->from = $saturday;
      $tmp->to = $sunday;
      $tmp->type = $period->type;
      $tmp->className = $period->className;
      $periods[] = $tmp;
      $d += 7;
    } while ($d <= $nb_days);
    unset($periods[$k]);
  }
}
$lines = array();
foreach (I18N::$days[Locale::FR]['short'] as $k => $v) {
  $lines[0][] = $v;
}

$day_count = cal_days_in_month(CAL_GREGORIAN, $month, $year);
$first_monday_day_num = (int) date('N', mktime(0, 0, 0, $month, 1, $year));
$week_count = 5;

if ($day_count == 30 && $first_monday_day_num == 7) { $week_count++; }
elseif ($day_count == 31 && $first_monday_day_num >= 6) { $week_count++; }
elseif ($day_count == 28 && $first_monday_day_num == 1) { $week_count--; }

$num = 1;
for ($i = 1; $i <= $week_count; $i++) {
  for ($j = 1; $j <= 7; $j++) {
    //    $day_num = $j + (($i - 1) * 7);
    if (($i === 1 && $j < $first_monday_day_num) || $num > $day_count) {
      $lines[$i][$j]['num'] = 0;
    }
    else {
      $uts = mktime(0, 0, 0, $month, $num, $year);
      $type = $default_type;
      $className = $num == $day ? array('selected') : array();
      if ($uts >= $from &&
          $uts <= $to) {
        $type = 'allow';
      }
      foreach ($periods as &$period) {
        if ($uts >= $period->from &&
            $uts <= $period->to) {
          $type = $period->type;
          $className[] = $period->className;
        }
      }
      $className = array_unique($className);
      $lines[$i][$j] = array('num' => $num,
                             'date' => str_pad($num, 2, '0', STR_PAD_LEFT).'/'.$month.'/'.$year,
                             'type' => $type,
                             'className' => $className);
      $num++;
    }
  }
}

$ctx->has_time = $has_time;
$ctx->empty_button = $empty_button;
$ctx->minute = $minute;
$ctx->hour = $hour;
$ctx->day = $day;
$ctx->month = $month;
$ctx->year = $year;
$ctx->lines = $lines;

$res->useTemplate();