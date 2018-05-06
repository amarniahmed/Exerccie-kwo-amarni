<?php

try {

  $uid = $req->getUserId();

  $res->sendRedirect('/account');

}
catch (AuthException $e) { $res->sendAuthenticationForm(); }
catch (Exception $e) { $err->add($e); }