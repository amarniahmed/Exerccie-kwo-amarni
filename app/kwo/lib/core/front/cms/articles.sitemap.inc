<?php

try {

  $opts = array('filters' => $filters,
                'order' => array('published_at' => 'DESC'));
  $articles = Article::collect($opts);

  $ctx->articles = $articles;

  $res->resetTemplates();
  $res->setContentType('xml');
  $res->write($ctx->articles->asSitemap());

}
catch (Exception $e) { $err->add($e); }
