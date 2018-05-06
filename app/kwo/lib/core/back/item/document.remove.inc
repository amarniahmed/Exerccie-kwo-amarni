<?php

try {

  $item = Item::getInstance(MODEL, (int) $req->id);

  Document::remove($item);

  $ctx->callback_notification = l('le document a bien été supprimé de l’index');

}
catch (Exception $e) { $err->add($e); }