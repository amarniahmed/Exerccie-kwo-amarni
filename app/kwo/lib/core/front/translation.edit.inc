<?php

try {

  if (!$req->isAuthenticated()) {
    throw new AuthException();
  }

  $user = $req->getUser();

  if (!$user->hasProfile('translator')) {
    throw new Exception('user is not translator');
  }

  $translator = $user->getProfile('translator');
  $locales = $translator->getLocales();

  if (count($locales) < 1) {
    throw new Exception('translator has no locales');
  }

  $translation = Translation::getInstance((int) $req->id);

  if (!$translator->hasFlag($translation->locale, 'locales')) {
    throw new Exception('translator is not allowed to use locale ('.$translation->locale.')');
  }

  $item = $translation->getItem();


  $ctx->locale = $translation->locale;
  $ctx->translation = $translation;
  $ctx->item = $item;


  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }