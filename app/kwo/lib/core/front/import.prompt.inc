<?php

try {

  $ctx->providers = ImportContacts::$providers;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
