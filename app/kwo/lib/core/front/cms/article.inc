<?php

try {

  $article = Article::getInstance((int) $req->id);
  
  if (!$article->isOnline()) {
    $res->sendRedirect('/articles');
  }
  
  $article->track();

  $chapters = $article->getChapters();
  
  $title = $article->name;
  
  $breadcrumb = array($article);
  
  if ($chapters->count() >= 1) {
    $chapters->offset = (int) $req->offset;
    $chapter = $chapters->skip((int) $req->offset)->current();
    $ctx->chapter = $chapter;
    $title .= ' | '.$ctx->chapter->name;
    $ctx->chapters = $chapters;
    $ctx->offset = $req->offset;
    if ($req->offset >= 1) {
      $breadcrumb[] = $chapter;
    }
  }

  $req->track($article);

  $ctx->article = $article;

  $res->setHead($title);
  $res->setBreadcrumb($breadcrumb);
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
