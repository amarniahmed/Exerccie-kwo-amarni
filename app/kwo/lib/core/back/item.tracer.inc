<?php

try {

  setcookie(Request::COOKIE_TRACE_NAME, intval(date('His')), 0, '/');

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
