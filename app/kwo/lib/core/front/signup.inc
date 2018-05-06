<?php

try {

  $res->sendAuthenticationForm();

}
catch (Exception $e) { $err->add($e); }