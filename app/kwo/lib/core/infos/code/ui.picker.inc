<?php

try { 
  
  $res->write(new Picker($req->getAttributes()));

}
catch (Exception $e) { $err->add($e); }