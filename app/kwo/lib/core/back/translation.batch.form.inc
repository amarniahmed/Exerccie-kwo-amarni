<?php

try {

  $models = Translation::getLocalizedModels();
  unset($models['translator']);

  $locales = Translation::locales();

  $statuses = Translation::$statuses;

  $priorities = Translation::$priorities;

  $ctx->models = $models;
  $ctx->locales = $locales;
  $ctx->statuses = $statuses;
  $ctx->priorities = $priorities;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
