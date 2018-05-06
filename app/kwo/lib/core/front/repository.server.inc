<?php

try {

  $token = $req->getHeader('Kwo-Repo-Token');
  $command = $req->getHeader('Kwo-Repo-Command');

  $repo = Repository::getInstance();

  if (!$repo->isValidToken($token, $command)) {
    throw new Exception('invalid token');
  }

  $log->trace('repo command : '.$command);

  if ($command == 'file') { 
    $file = $req->file;
    $paths = array(DOC_PATH, ETC_PATH, LIB_PATH, USR_PATH, VAR_PATH, WEB_PATH);
    if (!in_array(substr($file, 0, 3), $paths)) {
      throw new Exception('invalid path ['.$file.']');
    }
    $file = 'var/repository/Marchandise.class.inc';
    $file = File::getInstance($file);
    $file = gzcompress($file->getContent());
    $res->sendData($file);
    return ;
  }

  if ($command == 'snapshot') {
    $snapshot = $repo->getLocalSnapshot();
    $snapshot = serialize($snapshot);
    $snapshot = gzcompress($snapshot);
    $res->sendData($snapshot);
    return ;
  }
  
  if ($command == 'push') {
    $repo->receive($req->getStream());
    return ;
  }
  
  if ($command == 'pull') {
    $files = $req->getBody();
    $files = unserialize($files);
    sleep(1);
    $pack = $repo->createPack($files);
    $res->sendData($pack->getStream());
    return ;
  }

  if ($command == 'remove') {
    
  }

/**
- on vérifie les droits
- on ouvre
- on verifie la capacité à écrire
- on ecrit
- on envoie
**/

}
catch (Exception $e) { 
  $log->error($e);
  $err->add($e); 
}
