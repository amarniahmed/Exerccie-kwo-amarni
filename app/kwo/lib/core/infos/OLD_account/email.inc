<?php

try {

  $res->setMenu('email');
  $res->setTitle(l('account.title.email'));

}
catch (Exception $e) { $err->add($e); }
