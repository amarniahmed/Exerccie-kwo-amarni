<?php

try {

  $res->setMenu('password');
  $res->setTitle(l('account.title.password'));

}
catch (Exception $e) { $err->add($e); }