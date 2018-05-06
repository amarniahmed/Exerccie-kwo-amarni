<?php

try {

  String::CacheWrite();

  $res->sendMessage(c(l('les dictionnaires ont bien été générés')));

}
catch (Exception $e) { $err->add($e); }