<?php

try {

  if ($req->id < 1) {
    throw new Exception(l('error.id.unknown'));
  }

  $user = User::getInstance((int) $req->id);

  if ($user->status < User::STATUS_CLOSED) {
    $log->trace('user '.$user->email.', invalid status');
    $res->sendRedirect('/');
    return ;
  }

  if ($req->key !== SH::crypt($user->email)) {
    throw new Exception(l('error.key.invalid'));
  }

  $user->setStatus(User::STATUS_CONFIRMED);

  $ctx->user = $user;

  $req->track();

  $res->setFlash('votre compte est validé');

  if (file_exists(LIB_PATH.'/'.P('app.extension').'/middle/user.edit.inc')) {
    $res->sendRedirect('/middle');
  }
  else {
    $res->sendRedirect('/account');
  }

}
catch (Exception $e) { $err->add($e); }
