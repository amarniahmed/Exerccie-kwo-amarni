<?php

try {

  $visit = Visit::getInstance((int) $req->id);

  $opts = array('filter' => $req->type);
  $visits = $visit->getSiblings($opts);

  $label = 'visites ';
  if ($req->type == 'ip') {
    $label .= 'de la même adresse IP';
  }
  elseif ($req->type == 'user') {
    $label .= 'du même user (((visit_user_id)))';
  }
  elseif ($req->type == 'source') {
    $label .= 'de la même source de trafic';
  }
  else {
    $label .= 'du même visiteur';
  }

  $ctx->visit = $visit;
  $ctx->label = c(l($label, array('visit_user_id' => $visit->user_id)));
  $ctx->visits = $visits;
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }