<?php

try { 

  $alert = Alert::getInstance($req->id);

  if (!$user->hasAccessTo($alert)) {
    throw new MaliciousException();
  }

  $alert->toggleFlag(Alert::FLAG_DELIVER_EMAIL, $req->email);
    
  $res->setFlash(l('flash.save.success'));

  $ctx->id = $alert->getId();

}
catch (Exception $e) { $err->add($e); }

