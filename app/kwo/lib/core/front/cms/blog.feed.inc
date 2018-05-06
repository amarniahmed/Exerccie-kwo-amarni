<?php

try { 

  $res->setContentType('rss');

  $args = null;
  if ($req->id >= 1) { 
    $args = (int) $req->id; 
  }
  else { 
    list ($args, ) = explode('.', $req->getServer('host')); 
  }

  $blog = Blog::getInstance($args);

  if (!$blog->isOnline() || $req->isPreview()) {
    throw new Exception(l('error.item.invalid'));
  }

  $res->write($blog->asFeed());
  
}
catch (Exception $e) { $err->add($e); }