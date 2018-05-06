<?php

try {

  $alert = Alert::getInstance($req->id);

  if (!$user->hasAccessTo($alert)) {
    throw new MaliciousException();
  }

  $alert->setStatus(Alert::STATUS_OFFLINE);

  $res->setFlash(l('flash.delete.success'));

}
catch (Exception $e) { $err->add($e); }
