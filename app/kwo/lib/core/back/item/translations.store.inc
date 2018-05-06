<?php

try {

  $item = Item::getInstanceByKey($req->item);

  if ($item->hasAttribute('about')) {
    $item->setAttribute('about', $req->about);
  }

  $translations = (array) $req->translations;

  foreach ($translations as $locale => $opts) {
    if ($opts['status'] < 1) continue ;
    Translation::add($item, $locale, $opts);
  }

}
catch (Exception $e) { $err->add($e); }
