<?php

try { 

  if (!$req->isModerator()) {
    throw new PrivilegeException(l('error.privilege'));
  }
  
  $topic = Item::getInstanceByKey($req->item); 

  $forum = Forum::getInstance((int) $req->forum_id);

  $topic->commit(array('forum_id' => $forum->getId()));

  $res->sendRedirect($topic);

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }