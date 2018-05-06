<?php

try {

  $group = Group::getInstance($req->id ? $req->id : true);
  
  if (!$user->hasAccessTo($group)) {
    throw new MaliciousException();
  }

  $group->destroy();

  $res->setFlash(l('item.deleted'));

  //$res->sendRedirect('group.list');

}
catch (Exception $e) { $err->add($e); }
