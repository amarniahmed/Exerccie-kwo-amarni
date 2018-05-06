<?php

try { 
  
  $report = Report::getInstance($req->period);

  $ctx->report = $report;

  $ctx->landings = $report->getAttribute('landing');
  $ctx->exits = $report->getAttribute('exit');
  $ctx->hosts = $report->getAttribute('host');
  $ctx->metrics = $report->getAttribute('metric');
  $ctx->browsers = $report->getAttribute('browser');
  $ctx->systems = $report->getAttribute('system');
  $ctx->countries = $report->getAttribute('country');
  $ctx->depth = $report->getAttribute('depth');

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
