<?php

try {

  $opts = array('locale' => true);
  $blog = Blog::getInstance((int) $req->id, $opts);

  $filters = array('status' => Post::STATUS_PUBLISHED);
  $opts = array('filters' => $filters,
                'last' => 10,
                'locale' => true);
  $posts = $blog->getPosts($opts);

  $filters = array('status' => Post::STATUS_PUBLISHED);
  $opts = array('filters' => $filters,
                'cache' => false);
  $posts = $blog->getPosts($opts);

  $opts = array('action' => $blog->asUrl().'/tag/((tag))',
                'filters' => $posts);
  $cloud = Cloud::getInstance($opts);

  $ctx->blog = $blog;
  $ctx->posts = $posts;
  $ctx->cloud = $cloud;
  $ctx->heading_id = (int) $req->heading_id;
  $ctx->query = $req->query;
  $ctx->archives = $blog->getArchives();
  $ctx->headings = $blog->getPostHeadings();

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }