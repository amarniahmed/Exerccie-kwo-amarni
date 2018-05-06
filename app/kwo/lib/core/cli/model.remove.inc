<?php

try {

  $name = $req->name;
  if (!Model::exists($name)) {
    throw new Exception('model does not exists');
  }

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
