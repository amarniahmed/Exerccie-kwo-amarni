<?php

try {

  $folder = Folder::getInstance($req->path);
  $folder->compress();

  $ctx->path = $req->path;

}
catch (Exception $e) { $err->add($e); }

