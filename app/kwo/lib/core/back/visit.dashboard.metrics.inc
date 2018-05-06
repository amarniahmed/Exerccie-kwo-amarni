<?php

try { 
  
  $report = Report::getInstance($req->period);

  $ctx->report = $report;

  $ctx->landings = $report->getAttribute('landing');
  $ctx->exits = $report->getAttribute('exit');
  $ctx->shares = $report->getAttribute('share');
  $ctx->metrics = $report->getAttribute('metric');
  $ctx->browsers = $report->getAttribute('browser');
  $ctx->systems = $report->getAttribute('system');
  $ctx->locales = $report->getAttribute('locale');
  $ctx->countries = $report->getAttribute('country');
  $ctx->depth = $report->getAttribute('depth'); 

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
