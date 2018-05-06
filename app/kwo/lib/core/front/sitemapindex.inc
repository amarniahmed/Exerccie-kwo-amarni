<?php

try {

  $res->setContentType('xml');

  $sitemaps = array();
  if (S('app')->hasManager()) {
    if (method_exists(S('app')->getManager(), 'sitemap')) {
      $sitemaps = call_user_func(array(S('app')->getManager(), 'sitemaps'), $type);
    }
  }
  
  if (empty($sitemaps)) {
    throw new Exception('no sitemap');
  }

  $res->write(Sitemap::index($sitemaps));

}
catch (Exception $e) { $err->add($e); }
