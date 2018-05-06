<?php

try {

  $locale = $req->locale >= 1 ? (int) $req->locale : Locale::FR;

  $locales = S('app')->getLocales();

  if (!in_array($locale, $locales)) {
    $log->trace('invalid locale ('.$locale.')');
    $locale = S('app')->getLocale();
  }

  if ($req->getVisitId() >= 1) {
    $req->getVisit()->setAttribute('locale', $locale);
  }

  $req->getCookie()->setAttribute('locale', $locale);

  if ($req->isAuthenticated()) {
    $req->getUser()->setAttribute('locale', $locale);
  }

  $req->track();

  $ctx->locale = Locale::toCode($req->locale);//$ctx->locale = I18N::locale2code($req->locale);

  if (!$req->isXhr()) {
    $res->sendRedirect($req->hasAttribute('fallback') ? $req->fallback : '/');
  }

}
catch (Exception $e) { $log->error($e); }