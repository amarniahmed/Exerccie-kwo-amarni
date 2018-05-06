<?php

try {

  $report = Report::getInstance($req->period);

  $h = $report->getMedium($req->medium_id);

  $ctx->campaigns = $h['campaign'];
  $ctx->sources = $h['source'];
  $ctx->terms = $h['term'];

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
