<?php

try {

  if ($req->isAuthenticated()) {
    $user = $req->getUser();
    if (!$user->hasProfile('translator')) {
      return $res->sendRedirect('/');
    }
    $translator = $user->getProfile('translator');
  }
  else {
    if (!$req->hasAttribute('item')) {
      return $res->sendRedirect('/');
    }
    $translator = Translator::getInstanceByKey($req->item);
    $user = User::getInstance($translator->user_id);
    AuthManager::onUserAuth($user, AuthManager::AUTH_TOKEN);
  }

  if (!$user->isOnline()) {
    return $res->sendRedirect('/');
  }

  $locales = $translator->getLocales();

  if (count($locales) < 1) {
    $log->trace($user->email.' does not have locales associated');
    return $res->sendRedirect('/');
  }

  $item = null;
  $locale = $locales[0];
  $rf = Translation::getReferenceLocale();

  if ($req->hasAttribute('locale', true) && in_array($req->locale, $locales))  {
    $locale = (int) $req->locale;
  }
  else {
    foreach ($locales as $i => $_locale) {
      if ($_locale == $rf) continue ;
      $locale = $_locale;
      break;
    }
  }

  if (!$translator->hasFlag($locale, 'locales')) {
    throw new Exception('translator is not allowed to use locale ('.$locale.')');
  }

  if ($req->hasAttribute('id', true)) {
    $item = Translation::getInstance((int) $req->id);
  }

  $tabs = array();

  foreach ($locales as $i => $_locale) {
    if ($_locale == $rf) continue ;
    $tabs[] = array('code' => $_locale,
                    'label' => I18N::$langs[$_locale],
                    'selected' => $_locale == $locale);
  }

  $ctx->locale = $locale;
  $ctx->tabs = $tabs;
  $ctx->item = $item;

  $templates = array(null, '/translator');

  $res->setHead(array('title' => 'KWO - Translator'));
  $res->useTemplates($templates);

}
catch (Exception $e) {
  $log->error($e);
  $err->add($e);
}