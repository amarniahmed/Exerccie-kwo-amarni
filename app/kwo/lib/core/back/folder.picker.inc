<?php

try { 

  $path = trim($req->path, '/');

  if (substr($path, 0, strlen(DOC_PATH)) != DOC_PATH) {
    $path = DOC_PATH;
  }

  //  $session = $req->getSession();
  $ctx->path = $path;
  /*  if (empty($path) && strlen($session->file_picker_path) > 3) {
    $session->file_picker_path
    }*/

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }

