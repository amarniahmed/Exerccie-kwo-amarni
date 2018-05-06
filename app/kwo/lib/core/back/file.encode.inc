<?php

try {

  $file = File::getInstance($req->file_path, 'w');
  $content = $file->getContent();
  $enc = SH::detectEncoding($content);
  if ($enc != 'UTF-8') {
//    $file->write(iconv($enc, 'UTF-8', $content));
    $file->write(mb_convert_encoding($content, 'UTF-8', $enc));
  }
  $ctx->file_path = $file->getName();

}
catch (Exception $e) { $err->add($e); }
