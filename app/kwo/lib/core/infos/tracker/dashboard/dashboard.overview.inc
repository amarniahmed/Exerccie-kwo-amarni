<?php

try {
  
  list ($year, $month, $day) = explode('-', $req->period);
  
  $year = (int) $year;
  $month = (int) $month;
  $day = (int) $day;

  $report = Report::getInstance($year.'-'.$month.'-'.$day);

  $ctx->year = $year;
  $ctx->month = $month;
  $ctx->day = $day;

  $ctx->report = $report;

  $ctx->metrics = $report->getAttribute('metric');
  $ctx->intents = $report->getAttribute('intent');
  $ctx->mediums = $report->getAttribute('medium');
  $ctx->events = $report->getAttribute('event');
  $ctx->conversions = $report->getAttribute('conversion');

  $ctx->link = '/back/tracker/graph?year='.$req->year.'&month='.$req->month.'&day='.$req->day.'&width=360&heigh=120&ts='.time();

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
