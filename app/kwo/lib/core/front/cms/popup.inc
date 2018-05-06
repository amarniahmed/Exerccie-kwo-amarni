<?php

try {

  if (!$req->isXhr() || $req->id < 1) return '';

  $popup = Popup::getInstance((int) $req->id);

  if (!$popup->isOnline()) return '';

  $req->track($popup);

  $ctx->popup = $popup;
  
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
