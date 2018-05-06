<?php

try { 

  if (!$req->getUser()->isModerator()) {
    throw new PrivilegeException(l('error.privilege'));
  }

  $item = Item::getInstanceByKey($req->item); 
  $item->setBit(Meta::BIT_PINNED);

  $req->track();

  $res->sendRedirect($item);

}
catch (AuthException $e) { $res->sendAuthenticationForm(); }
catch (Exception $e) { $err->add($e); }