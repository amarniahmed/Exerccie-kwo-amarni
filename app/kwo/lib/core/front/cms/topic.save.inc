<?php

try {  

  $user = $req->getUser();
  
  $row = array('name' => SH::clean($req->name),
               'content' => SH::filter($req->content));

  if ($req->id >= 1) {
    $topic = Topic::getInstance((int) $req->id,
                                array('include' => 'forum'));
    if (!($topic->user_id == $user->getId() || $user->isModerator())) {
      throw new PrivilegeException(l('error.privilege'));
    }
    $forum = $topic->getForum();
    $topic->commit($row);
    $res->sendRedirect($topic);
  }
  elseif ($req->forum_id >= 1) {
    $forum = Forum::getInstance((int) $req->forum_id,
                                array('metas' => 'bits'));
    if ($forum->hasBit(Meta::BIT_LOCKED) && !$req->isModerator()) {
      throw new PrivilegeException(l('error.privilege'));
    }
    $topic = $forum->addTopic($row);
    $res->sendRedirect($forum);
  }
  else {
    throw new Exception(l('error'));
  }
  
  $topic->setVisit($req);
  $topic->setUser($user); 
  
  $req->track();
  
}
catch (AuthException $e) { $res->sendAuthenticationForm(); } 
catch (PrivilegeException $e) { $err->add($e); } 
catch (Exception $e) { $err->add(l('error.fields.required.all')); }
