<?php

try {

  $file = File::getInstance($req->file_path);
  $zip  = new ZipArchive();
  $ret = $zip->open($file->getName());
  if ($ret != true) {
    throw new Exception(c(l('ouverture impossible du fichier zip')));
  }

  $files = array();
  for ($i = 0; $i < $zip->numFiles; $i++) {
    $details = $zip->statIndex($i);
    if (File::isMaliciousName($details['name'])) {
      $bad[] = basename($details['name']);
    }
    elseif (!preg_match('/\/$/i',$details['name'])) {
      $filename = basename($details['name']);
      $zip->renameIndex($i, $filename);
      $log->error($filename);
      $files[] = $filename;
    }
  }

  if (count($bad) >= 1) {
     $err->add(c(l('cette archive contient un/des fichier(s) interdit(s)')).' : '.join(',', $bad));
     return;
  }

  $ret = $zip->extractTo($file->getPath(), $files);
  if ($ret === false) {
    $err->add(c(l('extraction impossible')));
    return;
  }
  $zip->close();
  $file->unlink();

}
catch (Exception $e) { $err->add($e); }