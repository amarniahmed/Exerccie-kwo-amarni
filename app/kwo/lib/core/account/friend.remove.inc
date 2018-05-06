<?php

try {

  $friend = Item::getInstanceByKey($req->item);

  Network::removeConnexion($user, $friend);

}
catch (Exception $e) { $err->add($e); }
