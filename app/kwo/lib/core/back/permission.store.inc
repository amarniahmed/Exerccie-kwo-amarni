<?php

try {

  $user = User::getInstance((int) $req->user_id);
  $item = Item::getInstanceByKey($req->item);

  if ($req->hasAttribute('permissions')) {
    $permissions = (int)$req->permissions;
  }
  
  ACL::grantAccess($user, $item, $permissions);

}
catch (Exception $e) { $err->add($e); }
