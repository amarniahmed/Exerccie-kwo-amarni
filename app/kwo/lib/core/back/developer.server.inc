<?php

  // TODO : taille : db, session, var/users

try {

  if (!$admin->isSuperAdmin()) return ;

  $datas = array();
  $datas[l('name')] = $_SERVER['SERVER_NAME'];
  $datas['IP'] = $_SERVER['SERVER_ADDR'];
  $datas[l('load avg')] = join(' &middot; ', sys_getloadavg());
  $datas[l('web server')] = $_SERVER['SERVER_SOFTWARE'];
  $datas[l('PHP version')] = phpversion();
  $datas[l('MySQL version')] = $dbo->getVersion();

  $ctx->datas = $datas;

  $res->useXml();

}
catch (Exception $e) { $err->add($e); }