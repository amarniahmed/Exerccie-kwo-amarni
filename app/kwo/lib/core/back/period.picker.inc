<?php

try {

  //$req->debug();
  
  $ctx->from = $req->from;
  $ctx->to = $req->to;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
