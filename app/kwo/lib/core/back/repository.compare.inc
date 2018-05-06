<?php

try {

//  sleep(2);

  $server = 'http://www.test.kwo';
  $repo = Repository::getInstance($server);

  $diff = $repo->compare();

  if (empty($diff['files'])) return ;

  $push = $pull = $lines = array();
  foreach ($diff['files'] as $file => $h) {
    if ($h['remote']['status']) {
      $push[] = $file;
      $line = 'PUSH';
    }
    else {
      $line = 'PULL';
      $pull[] = $file;
    }
  }

  $ctx->files = $diff['files'];

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
