<?php

try {

  $heading = Heading::getInstance((int) $req->id);
  
  $opts = array('filters' => array('status' => 1),
                'limit' => 10,
                'offset' => (int) $req->offset,
                'order' => array('published_at' => 'DESC'));
  $articles = $heading->getArticles($opts);

  $req->track($heading);

  $ctx->heading = $heading;
  $ctx->articles = $articles;
  
  $res->setHead($heading->name.' | e-zine');
  $res->setBreadcrumb(array(Heading::$types[$heading->type] => '/ezine/headings?type='.$heading->type,
                          $heading));
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }