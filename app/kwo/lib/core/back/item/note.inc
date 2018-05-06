<?php

try {

  $item = Item::getInstance(MODEL, (int) $req->id);

  $types = array(Item::DATA_NOTE,
                 Item::DATA_COMMENT,
                 Item::DATA_ABOUT);

  $h = $dbo->asHash('SELECT type, value'
                    .' FROM '.Item::TABLE_DATA
                    .' WHERE model_id='.$item->getModelId()
                    .  ' AND record_id='.$item->getId()
                    .' AND type IN ('.join(',', $types).')');
  
  $ctx->note = $h[Item::DATA_NOTE];
  $ctx->comment = $h[Item::DATA_COMMENT];

  $res->useXml();

}
catch (Exception $e) { $err->add($e); }