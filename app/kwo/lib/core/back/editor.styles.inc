<?php

try {

  $opts = array('order' => array('tag_type'),
                'filters' => array('|<>|tag_type' => Stylesheet::TAG_LINK));
  $ctx->stylesheets = Stylesheet::collect($opts);
  
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }