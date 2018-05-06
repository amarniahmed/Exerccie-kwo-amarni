<?php

try {

  if ($req->password != $req->password_confirm) {
    throw new Exception(l('error.password.different'));
  }

  if (strlen($req->password) < 6) {
    throw new Exception(l('error.password.size'));
  }

  if ($req->getUser()->setPassword($req->password) === false) {
    throw new Exception(l('error'));
  }

  $res->setFlash(l('flash.password.changed'));

}
catch (Exception $e) {
  $req->track('password modification', Tracker::CATEGORY_ERROR);
  $err->add($e);
}