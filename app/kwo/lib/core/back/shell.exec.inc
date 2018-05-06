<?php

try {

  ob_start();
  eval($req->src);
  $output = ob_get_clean();

  $ctx->output = $output;

}
catch (Exception $e) { $err->add($e); }
