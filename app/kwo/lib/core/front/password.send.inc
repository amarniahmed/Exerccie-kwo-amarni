<?php

try {

  $user = User::getInstance(array('email' => $req->email),
                            array('ignore' => true));

  if ($user->status <= User::STATUS_CLOSED) {
    $log->trace('invalid user ['.$user->email.']');
    throw new Exception();
  }

  $args = array('credential' => $user->getCredential());
  if (true) {
    if (file_exists(LIB_PATH.'/'.P('app.extension').'/middle/user.edit.inc')) {
      $path = '/middle/user.edit';
      $args['tab'] = 'password';
    }
    else {
      $path = '/account/password';
    }
  }
  else {
    $path = 'http://'.$req->getServer('host').'/account/password';
  }
  $url = H::url($path, $args);

  $link = H::elt('a', l('click.here'), array('href' => $url));

  Letter::send('password.reset', $user,
               array('link' => $link),
               array('locale' => $res->getLocale()));

  $req->track();

  $res->sendMessage(c(l('message.password.sent')));

}
catch (Exception $e) { $err->add(l('error.email.unknown')); }
