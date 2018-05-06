<?php

try {

  $max = P('log.threshold') ? P('log.threshold') : 10240;

  $prefix = '';
  $s = filesize(Logger::ERR_FILE);
  if ($s > $max) {
    $offset = $s - $max;
    $prefix = '------ CONTENT TRUNCATED (filesize='.$s.' bytes) ------'."\n\n";
  }
  $content = $prefix.file_get_contents(Logger::ERR_FILE, false, null, $offset);
  $ctx->code = str_ireplace(array('textarea'), array(' _TEXT_AREA_ '), $content);

  $prefix = '';
  $s = filesize(Logger::URL_FILE);
  if ($s > $max) {
    $offset = $s - $max;
    $prefix = '------ CONTENT TRUNCATED (filesize='.$s.' bytes) ------'."\n\n";
  }
  $ctx->url = file_get_contents(Logger::URL_FILE, false, null, $offset);

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
