<?php

try {

  $user = null;
  $user_id = 0;
  $item = Item::getInstanceByKey($req->item);
  if ($req->user_id > 0) {
    $user = User::getInstance((int) $req->user_id);
    $user_id = $user->getId();
    $permissions = ACL::getPermissions($user, $item);
  }

  $ctx->user = $user;
  $ctx->user_id = $user_id;
  $ctx->item = $item;
  $ctx->permissions = $permissions;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
