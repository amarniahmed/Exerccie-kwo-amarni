<?php

$year  = (int) $req->year;
$month = (int) $req->month;
$day   = (int) $req->day;

if (!($year > 1 && $month > 1 && $day > 1)) {
  $year  = (int) $req->year;
  $month = (int) $req->month;
  $day   = (int) $req->day;

  if ($year < 1) {
    $yesterday = mktime(0, 0, 0, date('m'), date('d') - 1, date('Y'));
    $year  = date('Y', $yesterday);
    $month = date('m', $yesterday);
    $day   = date('d', $yesterday);
    $rs = $dbo->query('SELECT day, month, year'
                      .' FROM tkr_cache'
                      .' WHERE year<>0 AND month<>0 AND day<>0'
                      .' ORDER BY year DESC, month DESC, day DESC'
                      .' LIMIT 1');
    if ($rs !== false && $rs->numRows() === 1) {
      $day   = $rs->result(0, 0);
      $month = $rs->result(0, 1);
      $year  = $rs->result(0, 2);
    }
  }
  elseif ($month < 1) {
    $rs = $dbo->query('SELECT month'
                      .' FROM tkr_cache'
                      .' WHERE year="'.$year.'" AND month>=1'
                      .' ORDER BY month ASC'
                      .' LIMIT 1');
    $month = $rs->result();
    if ($month >= 1 && $day == 0) {
      $rs = $dbo->query('SELECT day'
                        .' FROM tkr_cache'
                        .' WHERE year="'.$year.'" AND month="'.$month.'"'
                        .      ' AND day>=1'
                        .' ORDER BY day ASC'
                        .' LIMIT 1');
      $day = $rs->result();
    }
  }
  elseif ($day < 1) {
    $rs = $dbo->query('SELECT MIN(day)'
                      .' FROM tkr_cache'
                      .' WHERE year="'.$year.'" AND month="'.$month.'"'
                      .      ' AND day>=1'
                      .' ORDER BY day ASC'
                      .' LIMIT 1');
    $day = $rs->result();
  }
}

$ctx->year  = $year;
$ctx->month = $month;
$ctx->day   = $day;

$ctx->bloc_global = TrackerManager::renderBloc(0, 0, 0);
$ctx->bloc_year   = TrackerManager::renderBloc($year, 0, 0);
$ctx->bloc_month  = TrackerManager::renderBloc($year, $month, 0);
$ctx->bloc_day    = TrackerManager::renderBloc($year, $month, $day);

$res->useTemplate();
