<?php

try {
  
  $file = File::getInstance($req->file);

  $ctx->file = $file;
  
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
