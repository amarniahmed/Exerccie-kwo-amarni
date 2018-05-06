<?php

try { 

  $report = Report::getInstance($req->period);

  $grids = $report->getIntent($req->model_id);

  $ctx->grids = $grids;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }