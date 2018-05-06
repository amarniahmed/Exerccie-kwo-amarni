<?php

try {

  $forum = Forum::getInstance((int) $req->id);

  $forum->track();  
  
  $opts = array('filters' => array('forum_id' => $forum->getId()),
                'limit' => 20,
                'offset' => (int) $req->offset,
                'include' => 'user',
                'metas' => array(Meta::FIELD_COMMENTS, Meta::FIELD_DISPLAYS, Meta::FIELD_BITS, 
                                 'child', 'created_at', 'updated_at'),
                'order' => array('id' => 'DESC'));

  if ($req->offset < 1) {
    $opts['cols'] = array('('.Meta::FIELD_BITS.' & '.Meta::BIT_PINNED.') > 0' => 'is_pinned');
    $opts['order'] = array('is_pinned' => 'DESC',
                           'id' => 'DESC');
  }
  
  $ctx->forum = $forum;
  $ctx->topics = $forum->getTopics($opts); 
  $ctx->is_locked = $forum->hasBit(Meta::BIT_LOCKED) && !$req->isModerator();

  $res->setHead($forum.' | Forum');
  $res->setBreadcrumb(array('Forum' => '/board/forums',
                            $forum));
  $res->useTemplate(); 

}
catch (Exception $e) { $err->add($e); }
