<?php

try {

  $filepath = $req->filepath;

  if (strpos($filepath, '://') > 1) {
    $filepath = parse_url($filepath, PHP_URL_PATH);
  }

  $filepath = trim($filepath, '/');

  if (!file_exists($filepath)) {
    throw new Exception('['.$filepath.'] invalid path');
  }

  $ctx->file_path = $filepath;
  $ctx->file_size = SH::toSize(filesize($filepath));

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
