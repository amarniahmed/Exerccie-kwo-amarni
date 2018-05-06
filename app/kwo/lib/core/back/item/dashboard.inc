<?php

try {

  $ctx->model = MODEL;
  $ctx->date = $req->date;

  $res->useTemplate('core:item.dashboard');

}
catch (Exception $e) { $err->add($e); }