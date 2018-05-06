<?php

try { 

  $report = Report::getInstance($req->period);

  $h = $report->getCommunity();

  $ctx->sexes = $h['sexes'];
  $ctx->ages = $h['ages'];
  $ctx->origins = $h['origins'];
  $ctx->logins = $h['logins'];
  $ctx->intents = $h['intents'];
  $ctx->auths = $h['auths'];
  $ctx->countries = $h['countries'];

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
