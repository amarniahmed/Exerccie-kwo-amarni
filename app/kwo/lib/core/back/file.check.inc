<?php

try {

  $remote_file_path = trim($req->remote_file_path, '/');

  $folder = Folder::getInstance($req->path);
  if (!$folder->isWritable($req->path)) {
    throw new Exception(c(l('écriture impossible dans ce répertoire')));
  }

  $sep = strpos($remote_file_path, '\\') !== false ? '\\' : '/';

  $parts = explode($sep, $remote_file_path);

  $remote_file_path = $parts[count($parts) - 1];
  $remote_file_path = File::NameClean(basename($remote_file_path));

  if (File::isMaliciousName($remote_file_path)) {
    throw new Exception(c(l('cette extension n’est pas authorisée')));
  }

  $ctx->has_file = (int) $folder->hasFile($remote_file_path);
  $ctx->has_file_confirm = c(l('êtes-vous sûr de vouloir écraser le fichier distant ?'));

}
catch (Exception $e) { $err->add($e); }