<?php

try {
  
  $conversion = Item::getInstanceByKey($req->item); 

  if ($conversion->visit_id < 1) return ;

  $visit = Visit::getInstance($conversion->visit_id); 
  
  $opts = array('days' => 30);
  $visits = $visit->getSiblings($opts);

  $ctx->visit_id = $conversion->winner_id; 
  $ctx->visits = $visits;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
