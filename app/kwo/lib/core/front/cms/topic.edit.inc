<?php

try { 

  $user = $req->getUser();

  if ($req->id >= 1) {
    $topic = Topic::getInstance(intval($req->id), 
                                array('include' => 'forum'));
    if (!($topic->user_id == $user->getId() || $user->isModerator())) {
      throw new PrivilegeException(l('error.privilege'));
    }
    $forum = $topic->getForum();
  }
  elseif ($req->forum_id >= 1) {
    $topic = Topic::getInstance();
    $forum = Forum::getInstance(intval($req->forum_id));
  }
  else {
    throw new Exception(l('error'));
  }

  $ctx->topic = $topic;
  $ctx->forum = $forum;
  
  $res->setHead(l('topic.add').' | '.$forum.' | Forum');
  $res->setBreadcrumb(array('Forum' => '/board/forums',
                            $forum,
                            l('topic.new')));
  $res->useTemplate(); 

}
catch (AuthException $e) { $res->sendAuthenticationForm(); } 
catch (Exception $e) { $err->add($e); } 
