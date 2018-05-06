<?php

try {

  $args = array('status' => 1);

  if ($req->id >= 1) {
    $args['id'] = $req->id;
  }
  elseif (strpos($req->getServer('host'), '.blogs.') === false) {
    list ($args['code'], ) = explode('.', $req->getServer('host'));
  }

  $opts = array('locale' => $res->getLocale());
  $blog = Blog::getInstance($args, $opts);

  if (!$blog->isOnline() || $req->isPreview()) {
    throw new Exception(l('error.item.invalid'));
  }

  $blog->track();

  if (!$req->hasAttribute('id') && $blog->getId() >= 1) {
    $res->sendRedirect($blog->asUrl());
    return ;
  }

  $res->setHead($blog->name.' | Blog');

  $filters = array('status' => Post::STATUS_PUBLISHED);

  $opts = array('page' => (int) $req->page,
                'metas' => array(Meta::FIELD_COMMENTS),
                'order' => array('published_at' => 'desc'));

  if ($req->y > 2000) {
    $m = $req->hasAttribute('m', true) ? $req->m : date('n');
    $filters['|Y|published_at'] = $req->y;
    $filters['|M|published_at'] = $m;
    $last = true;
  }

  if (strlen($req->tag) >= 1) {
    $filters['|TAG|'] = $req->tag;
  }

  if ($req->heading >= 1) {
    $opts['tables'] = array(Heading::TABLE_POST => 'hp');
    $filters[] = 'hp.post_id='.T('post').'.id';
    $filters['hp.heading_id'] = $req->heading;
    $ctx->heading_id = (int) $req->heading;
  }

  if (!empty($req->query)) {
    $opts = array('limit' => 100,
                  'query' => $req->query,
                  'models' => array('post'),
                  'parent' => $blog->getId());
    $searcher = Searcher::getInstance($opts);
    $filters[T('post').'.id'] = $searcher->asSet();
    $ctx->query = $req->query;
  }

  $opts['last'] = $last;
  $opts['filters'] = $filters;
  $opts['locale'] = $res->getLocale();

  $posts = $blog->getPosts($opts);

  $ctx->blog = $blog;
  $ctx->posts = $posts;

  $res->addFeed('/blog.feed/-/id/'.$blog->getId(), 
                $blog->getName());
  $res->useTemplates('blog.skeleton', true);

}
catch (Exception $e) { $err->add(l('error.content.unknown')); }
