<?php

$directories = array(WEB_PATH, DOC_PATH, ETC_PATH,
                     LIB_PATH, PUB_PATH, USR_PATH);

$user = System::getApacheUser();
$user = $user['name'];

try {

  if (!Shell::confirm('Change owner to <'.$user.'>')) return ;

  if (posix_getpwnam($user) === false) {
    throw new Exception('user unknown');
  }

  Shell::exec('chown -R '.$user.':'.$user.' '.join(' ', $directories));

  if (is_dir('mysql')) {
    $user = System::getMysqlUser();
    $user = $user['name'];
    Shell::exec('chown -R '.$user.':'.$user.' mysql');
  }

}
catch (Exception $e) { $err->add($e); }