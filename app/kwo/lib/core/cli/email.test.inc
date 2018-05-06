<?php

try {

  $ret = Email::isValid($req->email, array('mx' => true, 'smtp' => true));

  echo $req->email.' : ';
  echo $ret ? 'ok' : 'ko';
  echo "\n";
   

}
catch (Exception $e) { $err->add($e); }
