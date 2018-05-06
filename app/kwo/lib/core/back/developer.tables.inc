<?php

try {

  if (!$admin->isSuperAdmin()) return ;

  $dbo->connect();
  $ctx->datas = $dbo->getTableSummary();

  $res->useXml();

}
catch (Exception $e) { $err->add($e); }
