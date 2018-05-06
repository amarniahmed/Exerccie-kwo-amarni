<?php

try {

  if (!$req->isAuthenticated()) {
    throw new Exception($ctx->_auth_errors);
  }

  $ctx->user_id = $req->getUser()->getId();

}
catch (Exception $e) { $err->add($e); }
