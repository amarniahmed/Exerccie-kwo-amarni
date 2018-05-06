<?php

try {

  $alert = Alert::getInstance((int) $req->id);

  if (!$user->hasAccessTo($alert)) {
    throw new MaliciousException();
  }

  $ctx->alert = $alert; 

}
catch (Exception $e) { $err->add($e); }
