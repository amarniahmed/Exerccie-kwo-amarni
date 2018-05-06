<?php

try {

  if (!$admin->isSuperAdmin()) return ;

  $models = array_flip(Model::all());
  $ctx->models_alpha = $models;

  asort($models);
  $ctx->models_num = $models;

  $res->useXml();

}
catch (Exception $e) { $err->add($e); }
