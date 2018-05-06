<?php

try {

  $user = User::getInstance((int) $req->user_id);
  $item = Item::getInstanceByKey($req->item);

  ACL::removeAccess($user, $item);

}
catch (Exception $e) { $err->add($e); }
