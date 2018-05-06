<?php

try {

  $files = (array) glob(Scheduler::PATH.'/*.*');

  $files = array_reverse($files);
  $files = array_flip($files);

  foreach ($files as $file => &$value) {
    $content = file_get_contents($file);
    $value = '';
    if (strpos($file, '.log') > 1) {
      $value = 'elt-warning';
    }
    elseif (strpos($content, 'failure')) {
      $value = 'elt-error';
    }
  }

  $ctx->files = $files;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
