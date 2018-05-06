<?php

  // TODO : backuper la db, grep, shell

try {

  if (!$admin->isSuperAdmin()) return ;

  $res->useXml();

}
catch (Exception $e) { $err->add($e); }
