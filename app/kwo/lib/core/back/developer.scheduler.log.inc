<?php

try {

  $filename = $req->filename;

  $ctx->content = file_get_contents(Scheduler::PATH.'/'.$filename);

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
