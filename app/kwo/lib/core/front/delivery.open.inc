<?php

try {

  Delivery::markAsOpened($req->knxdid);
  
}
catch (Exception $e) { $log->error($e); }

$res->close();

$res->setContentType('gif');
$res->write(SH::spacer());

