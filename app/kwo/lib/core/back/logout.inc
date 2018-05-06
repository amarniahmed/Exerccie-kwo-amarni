<?php

try {
  
  $req->getSession()->invalidate();

  $url = Admin::url();
  $res->sendRedirect('/'.$url);

}
catch (Exception $e) { $err->log($e); } 
