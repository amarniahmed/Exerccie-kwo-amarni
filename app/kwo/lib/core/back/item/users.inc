<?php

try { 

  $item = Item::getInstanceByKey($req->item);

  $rows = ACL::asPermissionRows($item);

  $res->write($rows);

}
catch (Exception $e) { $err->add($e); }
