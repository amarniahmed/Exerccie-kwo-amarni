<?php

try {

  $opts = array('locale' => $res->getLocale());
  $snippet = Snippet::getInstance('alert.prompt', $opts);

  $ctx->title = $snippet->title;
  $ctx->content = $snippet->content;
  
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
