<?php

try {

  $attrs = array('login', 'password');

  foreach ($attrs as $attr) {
    if (!$req->hasAttribute($attr, true)) {
      throw new Exception('missing attribute ['.$attr.'] in request', 
                          ApiManager::ERROR_BAD_REQUEST);
    }
  }

  $id = $dbo->fetchValue('SELECT id'
                        .' FROM '.T('user')
                        .' WHERE email=:login:'
                        .  ' AND password=MD5(:password:)'
                        .  ' AND status>'.User::STATUS_PENDING,
                        array(':login:' => $req->login,
                              ':password:' => $req->password));

  if ($id < 1) {
    throw new Exception('invalid credentials', ApiManager::ERROR_UNAUTHORIZED);
  }
  
  $user = User::getInstance($id);

  $ctx->access_token = ApiManager::getAccessToken($user);

}
catch (Exception $e) { $err->add($e); }