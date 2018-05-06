<?php

try {

  $folder = Folder::getInstance($req->path);
  $file_path = $folder->getParentPath();
  $folder->hide();

  $ctx->path = $file_path;

}
catch (Exception $e) { $err->add($e); }
