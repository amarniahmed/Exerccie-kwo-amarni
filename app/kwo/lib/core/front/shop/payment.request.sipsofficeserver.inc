<?php

try {
  Psp::load('sipsofficeserver');
  
  // S('log')->debug($req->mode);
  if ($req->hasAttribute('mode') && $req->mode == 'carte') {
    Sipsofficeserver::callback();
  }
  else {
    Sipsofficeserver::callbackPaypal();
  }
}
catch (Exception $e) { $err->add($e); }