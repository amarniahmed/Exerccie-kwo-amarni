<?php

class ArchiveManager 
{
  public static function compress($path) {
    $path = $path instanceof Folder ? $path->getPath() : rtrim($path, '/');
    $zip = new ZipArchive();
    $zip->open($path.'/'.basename($path).'.zip', ZipArchive::CREATE);
    self::addDir($path, $zip);
  }
  
  private static function addDir($path, $zip, $vpath=''){
    if (!is_dir($path) || !($dh = opendir($path))) return ;
    if (!empty($vpath)) $zip->addEmptyDir($vpath);
    while (($file = readdir($dh)) !== false) {
      if ($file[0] === '.') continue ;
      if (is_file($path.'/'.$file)) {
        $zip->addFile($path.'/'.$file, $vpath.$file);      
        continue ;
      }
      self::addDir($path.'/'.$file, $zip, $vpath.$file.'/');
    }
    closedir($dh);
  }
}