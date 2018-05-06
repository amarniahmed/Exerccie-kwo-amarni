<?php

try {

  $item = Item::getInstance(MODEL, $req->id);

  $n = $item->truncate();

  $msg  = l('opération réalisée avec succès').' : '."\n";
  $msg .= $n.' '.l('objets supprimé(s)');

  $res->sendMessage($msg);

}
catch (Exception $e) { $err->add($e); }