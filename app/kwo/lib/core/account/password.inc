<?php

try {

  $cookie = $req->getCookie();

  $auth_method = 'form';
  if ($cookie->hasAttribute('auth_method')) {
    $auth_method = $cookie->getAttribute('auth_method') ?: 'form';
  }

  $ctx->auth_method = $auth_method;

  $res->setMenu('password');
  $res->setTitle(l('account.title.password'));

}
catch (Exception $e) { $err->add($e); }