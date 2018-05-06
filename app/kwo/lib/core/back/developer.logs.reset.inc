<?php

try {

  if (!$admin->isSuperAdmin()) return ;

  Logger::clean();

  $res->sendMessage(c(l('opération réalisée avec succès')));

}
catch (Exception $e) { $err->add($e); }
