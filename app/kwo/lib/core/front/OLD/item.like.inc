<?php

try {

  $user = $req->getUser();

  $item = Item::getInstanceByKey($req->item);

  $table = $dbo->getTable(Interaction::TABLE);
  $row = array('model_id' => $item->getModelId(),
               'record_id' => $item->getId(),
               'type' => Interaction::TYPE_LIKE,
               'user_id' => $user->getId());
  if (!$table->hasRow($row)) {
    $item->track(Interaction::TYPE_LIKE, 0, array('user_id' => $user->getId()));
  }

  $n = $item->getMeta('`'.Meta::FIELD_LIKES.'`');

  $ctx->like_count = $n;

}
catch (AuthException $e) { $res->sendAuthenticationForm(); }
catch (Exception $e) { $err->add($e); }
