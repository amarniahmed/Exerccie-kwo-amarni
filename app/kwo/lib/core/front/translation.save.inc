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

  //$req->cleanAttributes();

  $translation = Translation::getInstance((int) $req->id);

  if ($translation->isValidated()) {
    throw new Exception('translat is already validated ('.$translation->getId().')');
  }

  if (!$translator->hasFlag($translation->locale, 'locales')) {
    throw new Exception('translator is not allowed to use locale ('.$translation->locale.')');
  }

  $record = (array) $req->record;

  if (!in_array($record['status'], array_keys(Translation::getEditorStatuses('edit')))) {
    $record['status'] = Translation::STATUS_DRAFT;
  }

  $translation->validate($record);
  $translation->commit($record);

  if ($req->md5 != $translation->getMd5() && $translation->isPendingValidation()) {
    $translation->setStatus(Translation::STATUS_DRAFT);
  }

  $ctx->id = $translation->getId();
  $ctx->status = $translation->status;

}
catch (Exception $e) { $err->add($e); }