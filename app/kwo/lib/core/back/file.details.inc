<?php

try {

  $file = File::getInstance($req->filepath);
  $details = $file->getDetails();
  $details['size'] = NH::asFileSize($details['size']);
  if ($details['ctime'] != $details['mtime']) {
    $details['mtime'] = date('Y-m-d G:i:s', $details['mtime']);
  }
  $details['ctime'] = date('Y-m-d G:i:s', $details['ctime']);
  if (isset($details['width'])) {
    $details['width'] .= ' px';
    $details['height'] .= ' px';
  }

  $ctx->file = $file;
  $ctx->details = $details;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
