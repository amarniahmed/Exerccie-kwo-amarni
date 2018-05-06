<?php

try {

  $user_id = 0;
  if ($req->isAuthenticated()) {
    $user_id = $req->getUserId();
  }

  $ctx->user_id = $user_id;

}
catch (Exception $e) { $err->add($e); }
