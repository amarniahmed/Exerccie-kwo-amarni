<?php

try {
  
  $item = Item::getInstance(MODEL, (int) $req->id);

  $hash = $dbo->asHash('SELECT *'
                       .' FROM '.Item::TABLE_SEO
                       .' WHERE model_id='.$item->getModelId()
                       .  ' AND record_id='.$item->getId(),
                       null, 'locale');

  foreach ($hash as $locale => &$row) {
    $row['tags'] = $item->getTags($locale);
  }

  $ctx->item = $item;
  $ctx->hash = $hash;

  $res->useXml();

}
catch (Exception $e) { $err->add($e); }