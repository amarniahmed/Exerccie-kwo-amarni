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
  $locale = (int) $req->locale;

  if (!$translator->hasFlag($locale, 'locales')) {
    throw new Exception('translator is not allowed to use locale ('.$locale.')');
  }

  $statuses = Translation::getEditorStatuses('list');
  $statuses = array_keys($statuses);
  $filters = array('locale' => $locale,
                   '|<|status' => Translation::STATUS_VALIDATED,
                   'status' => $statuses);

  if ($req->model_id > 0) {
    $filters['model_id'] = (int) $req->model_id;
  }
  
  if ($req->status > 0) {
    $filters['status'] = (int) $req->status;
  }

  $opts = array('filters' => $filters,
                'order' => array('priority' => 'desc',
                                 'status' => 'asc'));

  $translations = Translation::collect($opts);

  $ctx->translations = $translations;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }