<?php

try {

  if (!$admin->isSuperAdmin()) return ;

  $log->trace('cache clean');

  Cache::clean();

  $res->sendMessage(c(l('opération réalisée avec succès')));

}
catch (Exception $e) { $err->add($e); }