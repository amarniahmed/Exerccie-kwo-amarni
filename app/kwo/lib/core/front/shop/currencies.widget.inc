<?php

try {

  $opts = array('filters' => array('status' => 1),
                'cache' => true);
  $ctx->currencies = Currency::collect($opts);
  $res->useTemplate();

}
catch (Exception $e) { }