<?php

try { 

  $etc_path = '/etc/kernix/ftp';
  
  if (strlen($req->login) < 3) {
    throw new Exception('no login or login too short');
  }
  
  if (file_exists($etc_path.'/'.$req->login)) {
    throw new Exception('access already exists');
  }
  
  $ret = 
    '#Created by KWO
#'.DH::now().'
local_root='.System::pwd().'/app
local_umask=007
chown_username=apache
chown_uploads=yes
guest_username=apache';
  
  $res->write($ret);
  //  file_put_contents($etc_path.'/'.$req->login, $ret);
}
catch (Exception  $e) { $log->error($e); }