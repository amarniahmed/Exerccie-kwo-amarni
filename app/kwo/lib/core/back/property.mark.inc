<?php

try {

  $item = Item::getInstanceByKey($req->item);

  $model_id = $item->getModelId();
  $record_id = $item->getId();
  $property_id = (int) $req->property_id;

  $locales = P('app.locales');
  $locale = $locales[0];

  $dbo->update('UPDATE '.Item::TABLE_PROPERTY
               . ' SET status='.Property::STATUS_PENDING_TRANSLATION
               .' WHERE model_id='.$model_id
               .  ' AND record_id='.$record_id
               .  ' AND property_id='.$property_id
               .  ' AND locale<>'.$locale);

  $ctx->status = 1;

}
catch (Exception $e) { $err->add($e); }
