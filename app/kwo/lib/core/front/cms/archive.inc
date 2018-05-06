<?php

try {

  $month = $req->hasAttribute('month', true) ? $req->month : date('m');
  $year = $req->hasAttribute('year', true) ? $req->year : date('Y');

  $filters = array('|M|published_at' => $month,
                   '|Y|published_at' => $year);
  $opts = array('filters' => $filters,
                'include' => 'heading',
                'limit' => 10,
                'offset' => (int)$req->offset,
                'order' => array('published_at' => 'DESC'));
  $articles = Article::collect($opts);

  $ctx->articles = $articles;
  $ctx->year = (int)$year;
  $ctx->month = (int)$month;

  $res->setHead('archive');
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
