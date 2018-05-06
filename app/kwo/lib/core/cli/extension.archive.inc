<?php

try {

  Extension::getInstance($req->name)->archive();

}
catch (Exception $e) { $err->add($e); }