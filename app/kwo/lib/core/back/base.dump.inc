<?php

try {

  if ($req->id < 1) {
    throw new Exception(c(l('base inconnue')));
  }

  $dbo->exec('DELETE FROM '.Base::TABLE_EMAIL
             .    ' WHERE base_id='.$req->id);

  $n = $dbo->affectedRows();

  $res->sendMessage(c(l('la base a bien été vidée - adresse(s) supprimée(s)')).' : '.$n);

}
catch (Exception $e) { $err->add($e); }