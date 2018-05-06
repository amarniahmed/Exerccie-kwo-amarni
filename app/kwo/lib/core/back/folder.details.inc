<?php

try {

  $ctx->folder = Folder::getInstance($req->path);

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
