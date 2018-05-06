<?php

try {

  $item = Item::getInstance(MODEL, $req->id);

  $hash = array();
  foreach (I18N::locales() as $locale => $name) { 
    $hash[$locale]['tags'] = $item->getTags($locale);
  } 

  $ctx->item = $item;
  $ctx->hash = $hash; 

  $res->useXml();

}
catch (Exception $e) { $err->add($e); }