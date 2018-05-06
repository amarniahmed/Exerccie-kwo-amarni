<?php

try {

  $user = $req->getUser();

  $topic_id = $req->topic_id;
  $content = '';
  $comment_id = 0;

  if ($req->id >= 1) {
    $comment = Comment::getInstance((int) $req->id,
                                    array('include' => array('user')));
    if ($comment->model_id != Model::id('topic')) {
      throw new Exception(l('error'));
    }
    if ($req->mode === 'modify') {
      if (!($comment->user_id == $user->getId() || $user->isModerator())) {
        throw new PrivilegeException(l('error.privilege'));
      }
      $content = $comment->content;
      $comment_id = $comment->getId();
    }
    else {
      $content = '<br/><blockquote>';
      $content .= '<strong>'.$comment->user_name.'</strong> : <br/>';
      $content .= $comment->content;
      $content .= '</blockquote>';
    }
    $topic_id = $comment->record_id;
  }

  $topic = Topic::getInstance((int) $topic_id,
                              array('include' => 'forum'));
  $forum = $topic->getForum();

  $ctx->comment_id = $comment_id;
  $ctx->forum = $forum;
  $ctx->topic = $topic;
  $ctx->content = $content;

  $res->setHead(l('comment.add').' | '.$topic->name);
  $res->setBreadcrumb(array('Forum' => '/board/forums',
                            $forum,
                            $topic,
                            l('comment.new')));
  $res->useTemplate();

}
catch (AuthException $e) { $res->sendAuthenticationForm(); }
catch (Exception $e) {
  $log->error($e);
  $err->add($e);
}