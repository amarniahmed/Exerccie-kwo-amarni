<?php

try {

  $redirection = $ctx->redirection;
  
  $res->sendRedirect($redirection['destination'],
                     array(),
                     array('permanent' => $redirection['permanent']));

}
catch (Exception $e) { $err->add($e); }
