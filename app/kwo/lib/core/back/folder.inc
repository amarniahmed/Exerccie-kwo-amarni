<?php

try { 

  $path = $req->hasAttribute('path', true) ? $req->path : DOC_PATH;

  $folder = Folder::getInstance($path); 
  
  if ($req->hasAttribute('keyword', true)) {
    $files = $folder->getFiles(Folder::FILTER_REGEXP, $req->keyword);
  }
  elseif ($req->layout == 2) {
    $files = $folder->getFiles(Folder::FILTER_IMG);
  }
  else {
    $files = $folder->getFiles(); 
  }
  $files = array_merge($folder->getChildren(), $files); 
//  $log->debug($files);

  $columns = array(array(), array());
  $i = 0;
  foreach ($files as $file) {
    if ($file['is_folder'] == 1) {
      $elem = ' <a href="javascript:void(0)" onclick="Kwo.Dir.view(this);" data-path="'.$file['path'].'"><img src="'.PIX_URL.'/mime/dir.gif" />'.$file['name'].'</a>';
    }
    elseif ($req->layout==2) { 
      $elem = '<a href="javascript:void(0)" onclick="Kwo.File.preview(this)" data-path="/'.trim($path, '/').'/'.$file['name'].'" title="'.$file['name'].'"><img src="/'.$path.'/'.$file['name'].'" class="thumb" />'.$file['name'].'</a>';
      $elem .= ' ('.number_format($file['size'] / 1024, 1).'k)';
    }
    else { 
      $icon = Mime::getIcon($file['name']);
      $elem = '<a href="javascript:void(0)" onclick="Kwo.File.preview(this)" data-path="/'.trim($path, '/').'/'.$file['name'].'" title="'.$file['name'].'"><img src="'.PIX_URL.'/mime/'.$icon.'" />'.$file['name'].'</a>';
    }
//    error_log($elem);
    $columns[$i++%2][] = ' '.$elem;
  }

  $ctx->columns = $columns;
  $ctx->path = $folder->getPath();
  $ctx->can_delete = !$folder->isRoot();
  $ctx->selected_file_path = $req->getSession()->getAttribute('selected_file_path');
  $ctx->details = array('fichiers' => $folder->getNbFiles());

  $res->setTitle($folder->getHtmlPath());
  $res->useTemplate();
  
}
catch (Exception $e) { return $err->add($e); }

