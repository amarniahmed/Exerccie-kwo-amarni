<?php

try {

  $res->setContentType('xml');

  $file = basename($req->getPath(), '.xml');

  list (, $type) = explode('.', $file); 

  $req->setAttribute('type', $type);

  $set = array();
  if (S('app')->hasManager()) {
    if (method_exists(S('app')->getManager(), 'sitemap')) {
      $sitemap = call_user_func(array(S('app')->getManager(), 'sitemap'), $type);
      $res->write($sitemap->asXml());
    }
  }
  
}
catch (Exception $e) { $err->add($e); }
