<?php

try {

  if (!$req->hasAttributes('admin_login', 'admin_password', 'admin_url')) {
    $log->error('invalid request');
    throw new Exception();
  }
  
  if ($req->getAttribute('admin_url') != Admin::url()) { 
    $log->error('invalid admin url');
    throw new Exception();
  }

  $args = array('login' => $req->getAttribute('admin_login'), 
                'password' => $req->getAttribute('admin_password'));
  if (empty($args['login']) || empty($args['password'])) {
    throw new Exception();
  }
  $admin = Admin::getInstance($args); 
  
  $token = join('-', array($admin->getId(), SH::crypt($admin->getId())));

  $session = $req->getSession();
  $session->setAttribute('admin_token', $token);
  
  $cookie = $req->getCookie();
  $cookie->setAttribute('back.access', 1);

  $res->disableCache();
  $res->sendRedirect('/back/core/home');

}
catch (Exception $e) { $err->add($e); }
