<?php

try {

  $group = Group::getInstance($req->id);
  
  $group->removeUser($user);

  $res->sendMessage('coucou');

}
catch (Exception $e) { $err->add($e); }
