<?php

try {

  $item = Item::getInstance(MODEL, (int) $req->id);

  $item->index();

  $ctx->callback_notification = l('le document a bien été indexé');

}
catch (Exception $e) { $err->add($e); }