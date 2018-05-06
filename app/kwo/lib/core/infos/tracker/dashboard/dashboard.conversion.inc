<?php

try { 

  $report = Report::getInstance($req->period);

  $h = $report->getConversion($req->model_id);

  $ctx->mediums = $h['medium'];
  $ctx->campaigns = $h['campaign'];
  $ctx->sources = $h['source'];
  $ctx->terms = $h['term'];


  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
