<?php

try {

  $user = $req->getUser();

  $friend = Item::getInstanceByKey($req->item);

  if (!$friend->isA('user')) {
    throw new MaliciousException();
  }

  $ctx->user = $user;
  $ctx->friend = $friend;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
