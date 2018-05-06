<?php

try {

  if (!Extension::exists($req->name)) {
    throw new Extension('invalid name ['.$req->name.']');
  }

  if ($req->confirm != 'no' && !Shell::confirm('Are you sure', true)) return ;

  $extension = Extension::getInstance($req->name);
  
  if ($req->archive === 'yes' || empty($req->archive)) {
    $extension->archive();
  }

  $extension->uninstall();

}
catch (Exception $e) { $err->add($e); }