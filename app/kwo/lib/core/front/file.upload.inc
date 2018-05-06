<?php

try {

//  sleep(1);

  $h = array('user_id' => 0,
             'files' => array());

  if ($req->isAuthenticated()) {
    $user = $req->getUser();
    $h['user_id'] = $user->getId();
    $files = $user->receive();
    if (count($files) < 1) {
      throw new Exception('file not found');
    }

    foreach ($files as $file) {
      if ($req->filter === 'image' && !File::isImage($file)) {
        throw new Exception('invalid image');
      }
      $h['files'][] = $file->getName();
    }
  }
  else {
//    $req->debug(true);
    $name = key($_FILES);
    $file = UploadManager::anonymous($name);
    $h['files'][] = $file->getName();
  }

  $req->track('upload');

}
catch (Exception $e) {
  $log->error($e);
  $h['error'] = 'erreur '.$e->getMessage();
}

$json = json_encode($h);

$res->resetTemplates();
$res->write('<script src="'.WEB_URL.'/core/scripts/vendors/prototype.js" type="text/javascript"></script>');
$res->write('<script>window.onload = function() { $(top.document).fire("user:upload:completed", '.$json.'); }</script>');
