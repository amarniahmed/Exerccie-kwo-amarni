<?php

try {

  $visit = Visit::getInstance($req->id);

  $ip = IH::num2ip($visit->ip);

  $ctx->objects = array('inetnum' => array('inetnum', 'netname', 'country', 'descr'));
  $ctx->h = IH::ip2attrs($ip);
  
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
