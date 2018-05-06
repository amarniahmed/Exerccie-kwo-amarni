<?php

try { 

  $item = Item::getInstance(MODEL, (int) $req->id);

  foreach ($req->tags as $locale => $tags) {
    $item->setTags($tags, $locale);
  }

}
catch (Exception $e) { $err->add($e); }