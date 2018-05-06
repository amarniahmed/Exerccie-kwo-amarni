<?php

try {

  $user = $req->getUser();

  if ($req->hasAttribute('password_actual') && 
      !$user->checkPassword($req->password_actual)) {
    throw new Exception(l('error.password.actual.invalid'));
  }

  if ($req->password_new != $req->password_new_confirm) {
    throw new Exception(l('error.password.different'));
  }

  if (strlen($req->password_new) < 6) {
    throw new Exception(l('error.password.size'));
  }

  if ($user->setPassword($req->password_new) === false) {
    throw new Exception(l('error'));
  }

  $res->setFlash(l('flash.password.changed'));

}
catch (Exception $e) {
  $req->track('password modification', Tracker::CATEGORY_ERROR);
  $err->add($e);
}