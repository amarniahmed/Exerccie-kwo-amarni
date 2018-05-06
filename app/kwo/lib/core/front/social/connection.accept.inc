<?php

try {

  $user = $req->getUser();
  $user_id = (int) $req->user_id;

  if ($user_id < 1) {
    throw new Exception(l('error'));
  }

  $row = array('user_id' => $user->getId(),
               'friend_id' => $user_id);
  

}
catch (Exception $e) { $err->add($e); }
