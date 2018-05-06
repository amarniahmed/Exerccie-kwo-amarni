<?php

$session = $req->getSession();

try {

  $file = new File($session->getAttribute('selected_file_path'));

  $file->moveIn($req->path);

  $session->removeAttribute('selected_file_path');
  
  $res->write('Kwo.Dir.refresh()');
}
catch (Exception $e) { $err->add($e); }

