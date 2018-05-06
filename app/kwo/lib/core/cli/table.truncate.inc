<?php

try {

  if (!$dbo->hasTable($req->table, false)) return ;

  $dbo->getTable($req->table)->truncate();

}
catch (Exception $e) { $err->add($e); }
