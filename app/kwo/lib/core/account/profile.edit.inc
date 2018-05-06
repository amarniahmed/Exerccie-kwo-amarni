<?php

$res->setMenu('profile');
$res->setTitle(l('account.title.profile'));

try {

  $ctx->user = $user;

  $ctx->profile = $user->hasProfile()
                ? $user->getProfile()->getAttributes()
                : array();
}
catch (Exception $e) { $err->add($e); }
