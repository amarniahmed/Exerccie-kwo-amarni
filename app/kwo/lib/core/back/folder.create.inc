<?php

try {

  $path = Folder::PathClean($req->path);
  $path = empty($path) ? DOC_PATH : $path;

  $name = Folder::NameClean($req->name);

  $folder = $path.'/'.$name;

  if (Folder::exists($folder)) return ;

  if (!is_writable($path)) {
    throw new Exception(c(l('impossible d’écrire dans le dossier de destination')));
  }

  if (!Folder::create($folder, false)) {
    throw new Exception(c(l('une erreur est survenue durant la création du dossier')));
  }

  $ctx->folder_path = $folder;

}
catch (Exception $e) { $err->add($e); }