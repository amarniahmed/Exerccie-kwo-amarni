<?php

class DownloadManager 
{
  public static function file($file, $opts=null) {
    $req = S('req'); 
    $res = S('res'); 
    $opts = (array) $opts;
    if ($file instanceof File) {
      $file_path = $file->getName();
    }
    elseif ($file instanceof ActiveRecord) {
      $hash = $file->asDownloadHash($opts);
      $file_path = $hash['path'];
      $opts['filename'] = $hash['name'];
    }
    else {
      $file_path = $file;
    }
    $file_path = ltrim($file_path, '/');
    if (self::validate($file_path) !== true) {
      S('log')->error('path forbidden ['.$file_path.']', __METHOD__);
      throw new Exception('path forbidden');
    }
    if ($file instanceof ActiveRecord) {
      $file->track(Interaction::TYPE_DOWNLOAD);
    }
    if (strpos($file_path, 'stdout://') === 0) {
      $filename = strlen($opts['filename']) > 4 ? $opts['filename'] : 'fichier.data';
    }
    else {
      $filename = strlen($opts['filename']) > 4 ? $opts['filename'] : basename($file_path);
      $fp = fopen($file_path, 'r');
      if ($fp === false) {
        S('log')->error('can not open ['.$file_path.']', __METHOD__);
        throw new Exception('download error');
      }
    }
    
    $req->track($filename, 'download');

    $res->setContentType(Mime::getContentType($filename), $opts['charset']);
    if (!$req->isXhr()) {
      if ($req->isIe(6)) {
        $res->setHeader('Content-Disposition', 'inline; filename="'.$filename.'"');
        $res->setHeader('Cache-Control', 'must-revalidate');
      }
      else {
        $res->setHeader('Content-Disposition', 'attachment; filename="'.$filename.'"');
        $res->setHeader('Cache-Control', 'no-cache, must-revalidate');
      }
    }
    $res->setHeader('Content-Transfer-Encoding', 'binary');
    $res->setHeader('Accept-Ranges', 'bytes');
    $res->setHeader('Expires', 'Fri, 01 Jan 1990 00:00:00 GMT');
    $res->setHeader('Pragma', 'public');
    if (!is_resource($fp)) return ; 
    while (!feof($fp)) {
      $res->write(fread($fp, 2048));
    }
    fclose($fp);    
  }
  
  public static function validate($filename) {  
    if (strpos($filename, '..') !== false) return false; 
    if (strpos($filename, '~') !== false) return false; 
    if (stripos($filename, 'mem://') === 0) return true;
    if (stripos($filename, 'stdout://') === 0) return true;
    if (stripos($filename, 'http://') === 0) return true;
    if (stripos($filename, 'file://') === 0) {
      $filename = substr($filename, strlen('file://'));
      if (stripos($filename, '/tmp/') === 0) {
        return true;
      }
      $filename = trim($filename, '/ ');
      if (!is_file($filename)) { return false; }
      return true;
    }
    elseif (S('req')->isBack()) { 
      if (!in_array(substr($filename, 0, 3), 
                    array(DOC_PATH, PUB_PATH, USR_PATH, VAR_PATH))) { 
        return false;
      }
    }
    elseif (!in_array(substr($filename, 0, 3), 
                      array(DOC_PATH, PUB_PATH, USR_PATH))) {  
      return false;
    }
    if (!file_exists($filename)) return false;
    return true;
  }

}
