<?php

$res->reset();

$year  = (int) $req->year;
$month = (int) $req->month;
$day   = (int) $req->day;

if ($req->flag=='cmd') {
  $title = l('COMMANDES');
  $k1 = 'conversion';
  $k2 = 'cd';
}
elseif ($req->flag=='visitor') {
  $title = l('VISITEURS');
  $k1 = 'qty';
  $k2 = 'visitor';
}
else {
  $title = l('VISITES');
  $k1 = 'qty';
  $k2 = 'visit';
}

if ($year==0 && $month==0 && $day==0) {
  $flag = 'global';
  $title .= ' / '.l('ANNEE');
  $arr_name = 'year_arr';
}
if ($year!=0 && $month==0 && $day==0) {
  $flag = 'year';
  $title .= ' / '.l('MOIS');
  $arr_name = 'month_arr';
}
if ($year!=0 && $month!=0 && $day==0) {
  $flag = 'month';
  $title .= ' / '.l('JOUR');
  $arr_name = 'day_arr';
}
if ($year!=0 && $month!=0 && $day!=0) {
  $flag = 'day';
  $title .= ' / '.l('HEURE');
  $arr_name = 'hour_arr';
}

$report = Report::getInstance($year.'-'.$month.'-'.$day);

/*$rs = $dbo->query("SELECT ".$arr_name." FROM ".Report::TABLE." WHERE year=".$year." AND month=".$month." AND day=".$day);

if ($rs === false || $rs->numRows() < 1) {
  SH::spacer();
  return 0;
}

$tab = $rs->fetchValue();
$tabdata = @unserialize($tab);
$a_tmp = array();

foreach ($tabdata as $k => $v) {
  $a_tmp[$k] = $v[$k1][$k2];
}

if (sizeof($a_tmp)<1) {
  SH::spacer();
  return 0;
  }*/

$steps = $report->getSteps();

$width = $req->width ? $req->width : 300;
$height = $req->height ? $req->height : 150;

$chart = new Chart($width, $height);
//$chart->SetFontFile(Chart::FONT);
$chart->SetTitle($title);
$chart->SetValues($steps);
if ($p_flag=='cmd') $chart->SetColorBar("f5f0d3");

if ($flag=='day' || $flag=='month') $chart->SetAlternateKeys(true,1);
else $chart->SetAlternateKeys(false,0);

$chart->DisplayPNG();
//$chart->Distroy();