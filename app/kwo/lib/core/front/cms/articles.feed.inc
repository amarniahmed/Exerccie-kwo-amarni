<?php

try {

  $res->setContentType('rss');

  $filters = array('|>|status' => 0,
                   '|<=|published_at' => DH::now(),
                   'locale' => $res->getLocale());
  $opts = array('filters' => $filters,
                'order' => array('published_at' => 'desc'));
  $articles = Article::collect($opts);

  if ($articles->count() < 1) return ;
  
  $channel = $articles->first();

  $res->write($articles->asFeed($channel));

}
catch (Exception $e) {
  $err->add($e);
}