<?php

try { 

  $report = Report::getInstance($req->period);

  $grids = $report->getInteraction($req->model_id);

  $ctx->grids = $grids;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }