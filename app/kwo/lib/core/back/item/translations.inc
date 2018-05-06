<?php

try {

  $item = Item::getInstanceByKey($req->item);

  $opts = array('filters' => array('model_id' => $item->getModelId(),
                                   'record_id' => $item->getId()));

  $translations = array();

  foreach (Translation::collect($opts) as $translation) {
    $hash = array('status' => $translation->status,
                  'priority' => $translation->priority,
                  'content' => $translation->content);
    $translations[$translation->locale] = $hash;
  }


  $ctx->item = $item;
  $ctx->locales = Translation::locales();
  $ctx->translations = $translations;

  $res->useTemplate('core:item.translations');

}
catch (Exception $e) { $err->add($e); }
