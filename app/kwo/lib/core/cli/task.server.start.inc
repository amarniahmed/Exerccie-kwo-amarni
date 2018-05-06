<?php

try {

  TaskServer::getInstance();

  // here we are in the parent

}
catch (Exception $e) { $err->add($e); }
