<?php

try {

  if (!$req->hasAttribute('year')) {
    list ($year, $month, $day) = explode('-', DH::yesterday());
    $day = 0;
  }
  elseif ($req->year < 1) { 
    $year = $month = $day = 0;
  }
  elseif ($req->month < 1) {
    $year = (int) $req->year;
    $month = 1;
    $day = 0;
  }
  elseif ($req->day < 1) {
    $year = (int) $req->year;
    $month = (int) $req->month;
    if ($month === 0) { $month = 12; $year--; }
    elseif ($month === 13) { $month = 1; $year++; }
    $day = 1;
  }
  else {
    $year = (int) $req->year;
    $month = (int) $req->month;
    $day = (int) $req->day;
  }
  
  $weeks = array();
  
  $day_count = cal_days_in_month(CAL_GREGORIAN, $month, $year);
  $first_monday_day_num = (int) date('N', mktime(0, 0, 0, $month, 1, $year));
  $week_count = 5;
  
  if ($day_count == 30 && $first_monday_day_num == 7) { $week_count++; }
  elseif ($day_count == 31 && $first_monday_day_num >= 6) { $week_count++; }
  elseif ($day_count == 28 && $first_monday_day_num == 1) { $week_count--; }
  
  $num = 1;
  for ($i = 1; $i <= $week_count; $i++) {
    for ($j = 1; $j <= 7; $j++) {
      $day_num = $j + (($i - 1) * 7);
      if (($i === 1 && $j < $first_monday_day_num) ||
          $num > $day_count) { 
        $weeks[$i][$j] = 0;
      }
      else {
        $weeks[$i][$j] = $num;
        $num++;
      }
    }
  }
  
  $disabled_days = array();
  $months = I18N::$months[I18N::LOCALE_FR]['long'];

  if ($year == date('Y')) {
    $size = (int) date('m');
    if ($month >= date('m')) {
      $disabled_days = range(date('d'), 31);
    }
  }
  else {
    $size = 12;
  }
  $months = array_slice($months, 0, $size, true);

  $session = $req->getSession();
  if (true || !$session->hasAttribute('start_year')) {
    $start_year = $dbo->fetchValue('SELECT MIN(year)'
                                   .' FROM '.Report::TABLE
                                   .' WHERE year > 0'); 
    $start_year = $start_year < 2000 ? date('Y') : $start_year;
    $session->setAttribute('start_year', $start_year); 
  }

  $years = array('GLOBAL' => 0);
  foreach (range($session->getAttribute('start_year'), date('Y')) as $num) {
    $years[$num] = $num;
  }
  $ctx->years = $years;

  $ctx->disabled_days = $disabled_days;
  $ctx->months = $months;

  $ctx->day = $day;
  $ctx->month = $month;
  $ctx->year = $year;
  $ctx->weeks = $weeks; 
  
  $res->useTemplate();
  
}
catch (Exception $e) { $err->add($e); }
