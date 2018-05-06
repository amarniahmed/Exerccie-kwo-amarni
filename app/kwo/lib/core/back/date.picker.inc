<?php

try {

  //$req->debug();
  
  $is_date = false;

  if ($req->year > 1000) {
    $ts = mktime(0, 0, 0, $req->month, 1, $req->year);
    $date = date('Y-m-d', $ts);
  }
  elseif ($req->hasAttribute('date', true) && !DH::isNull($req->date)) {
    $date = $req->date;
    $is_date = true;
  }
  else {
    $date = DH::now();
  }
  $h = DH::asHash($date);

  $day   = $h['day'];
  $month = $h['month'];
  $year  = $h['year'];
  
  $days = array();
  foreach (I18N::$days[Locale::FR]['short'] as $k => $v) {
    $days[] = $v;
  }
  
  $day_count = cal_days_in_month(CAL_GREGORIAN, $month, $year);
  $first_monday_day_num = (int) date('N', mktime(0, 0, 0, $month, 1, $year));
  $week_count = 5;
  
  if ($day_count == 30 && $first_monday_day_num == 7) { $week_count++; }
  elseif ($day_count == 31 && $first_monday_day_num >= 6) { $week_count++; }
  elseif ($day_count == 28 && $first_monday_day_num == 1) { $week_count--; }
  
  $weeks = array();
  $num = 1;
  $m = str_pad($month, 2, '0', STR_PAD_LEFT);
  for ($i = 1; $i <= $week_count; $i++) {
    for ($j = 1; $j <= 7; $j++) {
      if (($i === 1 && $j < $first_monday_day_num) || $num > $day_count) {
        $weeks[$i][$j]['num'] = 0;
      }
      else {
        $weeks[$i][$j] = array('num' => $num,
                               'date' => $year.'-'.$m.'-'.str_pad($num, 2, '0', STR_PAD_LEFT),
                               'classes' => $num == $day && $is_date === true ? array('selected') : array());
        $num++;
      }
    }
  }
  
  $ctx->day = $day;
  $ctx->month = $month;
  $ctx->year = $year;
  $ctx->days = $days;
  $ctx->weeks = $weeks;
  $ctx->months = I18N::$months[Locale::FR]['long'];
  
  $res->useTemplate();

}
catch (Exception $e) {
  $log->error($e);
}