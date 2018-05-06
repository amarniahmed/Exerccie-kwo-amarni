<?php

try {

  $args = null;
  if ($req->id >= 1) {
    $args = (int) $req->id;
  }
  elseif ($req->hasAttribute('code', true)) {
    $args = array('code' => $req->code);
  }

  $post = Post::getInstance($args, array('include' => 'blog'));

  $blog = $post->getBlog();
  
  if (!($post->isOnline() && $blog->isOnline()) || $req->isPreview()) {
    throw new MaliciousException();
  }
  
  $post->track();
  
  $res->setHeader('X-Pingback', H::url('/xml.rpc'));

  $res->setHead($post);

  $ctx->post = $post;
  $ctx->blog = $blog;

  $res->useTemplates('blog.skeleton', true);

}
catch (Exception $e) { $err->add($e); }