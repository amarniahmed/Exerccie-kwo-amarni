<?php

try {

  if ($req->hasAttribute('file_path') && 
      strlen($req->file_path) >= strlen(DOC_PATH)) {
    $path = dirname($req->file_path);
  }
  elseif ($req->hasAttribute('path') && 
          strlen($req->path) >= strlen(DOC_PATH)) {

    $path = $req->path;
    if (strpos($path, 'http') === 0) {
      $hash = parse_url($path);
      $path = $hash['path'];
    }
    if (strpos($path, '.')) {
      $path = dirname($path);
    }

  }

  if (!preg_match('/(^|\/)'.preg_quote(DOC_PATH, '/').'/', $path)) {
    $path = DOC_PATH;
  }

  try {
    $folder = new Folder($path); 
  }
  catch (Exception $e) {
    $folder = new Folder(DOC_PATH); 
  }
  
  $files = array();
  if ($req->filter === 'image' || 
      $req->filter === 'images' || 
      $req->filter === 'img') {
    $files = $folder->getFiles(Folder::FILTER_IMG);
  }
  elseif ($req->filter === 'directory') {
    $files = $folder->getFiles(Folder::FILTER_DIR);
  }
  elseif ($req->filter === 'pdf') {
    $files = $folder->getFiles(Folder::FILTER_REGEXP, '\.pdf');
  } else {
    $files = $folder->getFiles();
  }
  
  $files = AH::concat($folder->getChildren(), $files);
  foreach ($files as &$file) {
    $file['ico'] = $file['is_folder'] == 1 ? 'dir.png' : Mime::getIcon($file['name']);
  }

  $ctx->files = $files; 
  $ctx->path = $folder->getPath();

}
catch (Exception $e) { $err->add($e); }
