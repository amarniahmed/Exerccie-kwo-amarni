<?php

try {

  if (!$req->isXhr()) {
    throw new MaliciousException();
  }
  
  $item = Item::getInstanceByKey($req->item);

  $type = $req->direction == 'up'
        ? Interaction::TYPE_LIKE
        : Interaction::TYPE_DISLIKE;

  $item->track($type);

}
catch (Exception $e) { $log->error($e); }

$res->sendMessage(l('thank_you'));