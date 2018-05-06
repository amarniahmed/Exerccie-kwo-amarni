<?php
try { 
  if (is_array($req->user)) {
    $user->update($req->user);
  }
  
  if (is_array($req->profile)) {
    $user->setProfile($req->profile);
  }

  $req->getSession()->auth($user);

  $res->setFlash(l('flash.save.success'));

}
catch (Exception $e) { $err->add($e); }
