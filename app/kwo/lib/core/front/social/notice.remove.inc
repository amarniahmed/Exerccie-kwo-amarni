<?php

try {

  if ($req->h != SH::crypt($req->nid . $req->uid)) {
    throw new Exception('invalid hash');
  }

  $user = User::getInstance((int) $req->uid);
  $notice = Notice::getInstance((int) $req->nid);

  $user->unsetFlag(NH::asBit($notice->getId()), 'notices');

  $msg = 'ok';
  try {
    $snippet = Snippet::getInstance('offer.unsubscribe.confirm');
    $msg = $snippet->getProperty('content');
  }
  catch (Exception $e) {
    $log->error($e);
  }

  $res->write($msg);

}
catch (Exception $e) { $err->add($e); }
