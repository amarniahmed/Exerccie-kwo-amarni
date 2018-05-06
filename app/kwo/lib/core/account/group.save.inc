<?php

try {

  $group = Group::getInstance($req->id > 0 ? $req->id : true);
  
  if (!$user->hasAccessTo($group)) {
    throw new MaliciousException();
  }

  $req->cleanAttributes();

  $name = $req->name;

  if (strlen($name) <= 1) {
    throw new Exception('error.fields.required');
  }

  $group->commit(array('name' => $name,
                       'description' => $req->description));

  if ($group->isNew()) {
    $group->setUser($user);
    $group->addUser($user);
  }

  $res->setFlash(l('item.saved'));

  $res->sendRedirect('group.list?group_id='.$group->getId());

}
catch (Exception $e) { $err->add($e); }
