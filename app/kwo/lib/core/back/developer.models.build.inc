<?php

try {

  if (!$admin->isSuperAdmin()) {
    $log->error('should be superadmin');
    return ;
  }

  if (!is_writable(ETC_PATH)) {
    throw new Exception('ETC_PATH is not writable');
  }

  ConfigManager::platform();

  $res->sendMessage(c(l('opération réalisée avec succès')));

}
catch (Exception $e) { $err->add($e); }