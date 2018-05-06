<?php

try {

  $user = User::getInstance((int) $req->id);

  if (!$user->isOnline()) {
    throw new Exception('error.profile.invalid');
  }

  if ($user->privacy > User::PRIVACY_PUBLIC) {
    if (!$req->isAuthenticated()) {
      throw new Exception('error.profile.privacy');
    }
    elseif (!Network::isConnected($user, $req->getUser())) {
      throw new Exception('error.profile.privacy');
    }
  }
  else {
    //throw new Exception('error.profile.privacy');
  }

  $profile = null;
  $title = 'profile';

  if ($user->hasProfile()) {
    $profile = $user->getProfile();
    $title = $profile->identity();
  }

  $ctx->user = $user;
  $ctx->profile = $profile;

}
catch (Exception $e) { }

$res->setHead($title);
$res->useTemplate();