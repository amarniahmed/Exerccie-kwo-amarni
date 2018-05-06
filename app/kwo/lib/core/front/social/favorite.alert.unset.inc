<?php

try {

  $user = $req->getUser();

  $favorite = Favorite::getInstance((int) $req->id);
  if (!$user->hasAccessTo($favorite)) {
    throw new MaliciousException();
  } 

  $favorite->unsetFlag(Favorite::FLAG_ALERT);

  $req->track();

}
catch (Exception $e) { $err->add($e); }
