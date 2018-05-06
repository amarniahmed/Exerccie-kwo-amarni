<?php

try { 

  $topic = Topic::getInstance(intval($req->id),
                              array('metas' => array('bits'),
                                    'include' => array('forum', 'user')));
  $topic->track();
  
  $forum = $topic->getForum();
  
  $opts = array('offset' => $req->offset,
                'limit' => 20,
                'order' => array('id'));
  $ctx->comments = $topic->getComments($opts);

  $ctx->topic = $topic;

  $ctx->forum = $forum;

  $ctx->is_locked = $topic->hasBit(Meta::BIT_LOCKED) && !$req->isModerator();

  $res->setHead($topic->name.' | '.$forum->name.' | Forum');
  $res->setBreadcrumb(array('Forum' => '/board/forums',
                            $forum,
                            $topic));
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }