<?php

try {

  $server = 'http://www.test.kwo';
  $repo = Repository::getInstance($server);

  $lines = $repo->diff($req->file);

//  foreach ($lines as $line) { break ; }

  $ctx->lines = array_splice($lines, 2);

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
