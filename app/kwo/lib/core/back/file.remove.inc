<?php

try {

  //$req->debug();

  $file = File::getInstance($req->filepath, 'w');

  $file->unlink();

  $ctx->path = $file->getPath();

}
catch (Exception $e) { $err->add($e); }
