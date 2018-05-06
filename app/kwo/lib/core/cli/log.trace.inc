<?php

try {

  $log->trace($req->message);

}
catch (Exception $e) { $err->add($e); }
