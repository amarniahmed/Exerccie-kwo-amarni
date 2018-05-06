<?php

try {

  $ctx->model = MODEL;

  $res->useTemplate('core:item.map');

}
catch (Exception $e) { $err->add($e); }
