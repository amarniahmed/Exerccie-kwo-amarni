<?php

try {

  $user = User::getInstance($req->id);

  ACL::removeAccess($user, Item::getInstanceByKey($req->item));

}
catch (Exception $e) { $err->add($e); }
