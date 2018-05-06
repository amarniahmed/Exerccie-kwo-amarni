<?php

$secret = SH::crypt(date('Y-m-d'));

if ($secret != $req->secret) {
  S('log')->error('invalid secret');
  return ;
}

$res->resetTemplates();

$h = array('server_version' => apache_get_version(),
           'server_modules' => apache_get_modules(),
           'server_user_name' => get_current_user(),
           'server_user_id' => getmyuid(),
           'php_display_errors' => ini_get('display_errors'),
           'php_register_globals' => ini_get('register_globals'),
           'mbstring.detect_order' => ini_get('mbstring.detect_order'),
           'mbstring.func_overload' => ini_get('mbstring.func_overload'),
           'default_charset' => ini_get('default_charset'),
           'php_safe_mode' => ini_get('safe_mode'),
           'php_register_globals' => ini_get('register_globals'),
           'php_session_auto_start' => ini_get('session.auto_start'),
           'php_magic_quotes_gpc' => get_magic_quotes_gpc(),
           'php_magic_quotes_runtime' => get_magic_quotes_runtime());

//$log->debug($h);

$res->write(serialize($h));