<?php

try {

  if ($req->hasSession()) {
    $session = $req->getSession();
    $session->logout();
    if ($session->isEmpty()) {
      $session->invalidate();
    }
    $req->track();
  }
  else {
    $req->track('logout', Tracker::CATEGORY_ERROR);
    S('log')->trace('logout failure, user has no session');
  }

  $req->getCookie()->removeAttribute('user_secret');

}
catch (Exception $e) {
  $log->error($e);
  $err->add($e);
}
