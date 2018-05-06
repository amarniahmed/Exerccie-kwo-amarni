<?php

try {

  $label = 'transfert de fichier';
  $user = null;
  $files = array();

  if ($req->isAuthenticated()) {
    $label = c(l('my.files'));
    $user = $req->getUser();
    $files = array();
    foreach ($user->getFiles() as $file) {
      if ($req->filter === 'image' && !File::isImage($file['name'])) continue ;
      $files[] = $file;
    }
  }

  $ctx->label = $label;
  $ctx->user = $user;
  $ctx->files = $files;

  $res->useTemplate();

}
catch (Exception $e) { }

/*
 if ($req->filter === 'image') {
    $metas = @getimagesize($file->getName());
    if ($metas === false ||
        !in_array($metas[2],
                  array(IMAGETYPE_GIF, IMAGETYPE_JPEG, IMAGETYPE_PNG))) {
      $log->error('invalid image (size)', $file->getName());
      $file->unlink();
      throw new Exception(l('error.image'));
    }
    if ($metas['channels'] == 4) {
      $log->error('CMYK image', $file->getName());
      $file->unlink();
      throw new Exception(l('error.image.channels'));
    }
  }
*/
