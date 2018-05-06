<?php

try {

  $server = 'http://www.test.kwo';
  $repo = Repository::getInstance($server);

  $diff = $repo->compare();

  print_r($diff);

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
    $lines[] = $line.' : '.$file;
  }

  sort($lines);

  $res->write(join("\n", $lines) . "\n\n");

  if (Shell::confirm('Proceed ?')) {
    $opts = array('push' => $push,
                  'pull' => $pull);
    $repo->sync($opts);
  }
  else {
    $res->write("\n". '... operation canceled' . "\n");
  }

}
catch (Exception $e) {
  $log->error($e);
  $err->add($e); 
}