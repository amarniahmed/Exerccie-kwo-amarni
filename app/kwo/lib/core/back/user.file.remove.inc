<?php

try { 

  $user = User::getInstance($req->id);

  $user->removeFile($req->filepath);

}
catch (Exception $e) { $err->add($e); }
