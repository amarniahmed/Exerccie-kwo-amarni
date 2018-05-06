<?php

try {

  $filepath = trim($req->file, '/');

  if ($req->filter === 'image') {
    if (!preg_match('/\.(jpg|jpeg|gif|png)$/i', $req->file)) {
      throw new Exception(l('error.image'));
    }
  }
  elseif (strlen($req->filter) > 1) {
    $regex = '/\.('.str_replace(',', '|', $req->filter).')$/i';
    if (!preg_match($regex, $req->file)) {
      throw new Exception('ce fichier est invalide,'.
                          ' seul sont acceptés les fichiers : '.$req->filter);
    }
  }

  if (!$req->isAuthenticated()) return ;

  /* TODO : verif quotas */

  if ($req->getUser()->hasFile($req->file) && $req->replace != 1) {
    throw new Exception('file exists', UploadManager::ERROR_FILE_EXISTS);
  }

}
catch (Exception $e) { $err->add($e); }