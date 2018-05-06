<?php

try {

  if ($req->hasAttribute('code', true)) {
    $snippet = Snippet::getInstance($req->code);
    $ctx->title = $snippet->getProperty('title');
    $ctx->content = $snippet->getProperty('content');
  }

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }