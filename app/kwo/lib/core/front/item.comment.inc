<?php

try { 

  $req->cleanAttributes();

  $user = $req->getUser();
  
  $content = $req->content;
  if (strlen($content) > 20000) {
    $content = substr($content, 0, 20000).'...';
    $req->track('comment truncated', Tracker::CATEGORY_ERROR);
    $log->trace('comment truncated');
  }

  $content = SH::filter($content);
  $content = SH::decorate($content);

  if ($req->id >= 1) {
    $comment = Comment::getInstance((int) $req->id, 
                                    array('include' => array('user')));
    if (!in_array($comment->model_id, array(Model::id('topic')))) {
      $log->trace('bad model_id ('.$comment->model_id.')');
      throw new Exception(l('error'));
    }
    if (!($comment->user_id == $user->getId() || $user->isModerator())) {
      throw new PrivilegeException(l('error.privilege'));
    }
    $item = $comment->getItem();
    $comment->commit(array('content' => $content));
    $req->track('comment.update');
  }
  else {
    $item = Item::getInstanceByKey($req->item, array('metas' => 'bits')); 
    if ($item->hasBit(Meta::BIT_LOCKED) && !$user->isModerator()) {
      throw new Exception(l('error.item.locked'));
    }
    $comment = $user->comment($item, $content);
    $comment->setVisit($req);
    $req->track('comment.save');
  }
  
  $res->sendRedirect($item);
  
}
catch (AuthException $e) { $res->sendAuthenticationForm(); }
catch (Exception $e) { 
  $log->error($e);
  $err->add($e); 
}
