<?php

try {

  if (!$req->isModerator()) {
    throw new PrivilegeException(l('error.privilege'));
  }
  
  $topic = Item::getInstanceByKey($req->item); 

  $ctx->topic = $topic;
  $ctx->forum = $topic->getForum();
  $ctx->forums = Model::asHash('Forum');

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }