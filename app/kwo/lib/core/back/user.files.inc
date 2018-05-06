<?php

try {

  $user = User::getInstance($req->id);

  $files = array();

  $ctx->files = $user->getFiles();
  $ctx->user = $user;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
