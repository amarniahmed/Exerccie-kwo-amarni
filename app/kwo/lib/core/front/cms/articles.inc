<?php

try {

  $filters = array('status' => Article::STATUS_PUBLISHED,
                   '|<=|published_at' => DH::now(),
                   'locales' => $res->getLocale());

  $opts = array('filters' => $filters,
                'limit' => 3,
                'offset' => (int) $req->offset,
                'order' => array('published_at' => 'DESC'));
  
  $articles = Article::collect($opts);

  $ctx->articles = $articles;

  $res->setHead('Articles');
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
