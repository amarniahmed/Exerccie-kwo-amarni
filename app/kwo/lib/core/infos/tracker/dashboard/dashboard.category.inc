<?php

try { 

  $report = Report::getInstance($req->period);

  $h = $report->getCategory($req->category_id);

  $ctx->actions = $h;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
