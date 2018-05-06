<?php

//$req->debug(true);

d($req->getHeaders());

if ($req->isXhr()) {

  try { 
    
    $filename = $req->getHeader('x-file-name');
    $path = $req->getHeader('x-file-destination');

    $filename = File::NameClean($filename);
    $path = Folder::PathClean($path); 

    if (empty($path) || !is_dir($path)) {
      if (!empty($path)) {
        $log->error('['.$path.'] path unknown');
      }
      $path = DOC_PATH;
    }
    
    $file_path = $path.'/'.$filename; 
    
    $in = $req->getStream();
    if (!is_resource($in)) {
      throw new Exception('invalid input');
    }
    $out = fopen($file_path, 'w');
    if (!is_resource($out)) {
      throw new Exception('invalid output');
    }
    while (!feof($in)) {
      fwrite($out, fread($in, 8192));
    }
    fclose($out);
    fclose($in);
    
  }
  catch (Exception $e) { 
    $log->error($e);
    $err->add($e); 
  }
  
  return ;
}

try { 
  $log->trace('DEPRECATED METHOD: **** warn fxb ****');
  $path = Folder::PathClean($req->path); 
  $folder = Folder::getInstance($path);
  UploadManager::back($folder);
  $res->sendRedirect('/back/core/folder', array('path' => $path));
}
catch (Exception $e) { 
  $log->error($e);  
}
