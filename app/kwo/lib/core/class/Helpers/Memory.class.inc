<?php

class Memory {

  const PATH = 'var/memory';

  public function get($key) {
    $this->checkKey($key);
    $path = self::PATH.'/'.$key.'.mem.inc';
    if (!file_exists($path)) return null;
    $data = file_get_contents($path);
    $data = unserialize($data);
    return $data[0];
  }

  public function set($key, $value) {
    $this->checkKey($key);
    $path = self::PATH.'/'.$key.'.mem.inc';
    return file_put_contents($path,
                             serialize(array($value)));
    
  }

  private function checkKey($key) {
    return (strpos($key, '..') != false && 
            strpos($key, '/') != false && 
            strpos($key, '\\') != false);
  }

  public static function clean() {
    if (strlen(self::PATH) <= 9 || !is_dir(self::PATH)) return ;

    $files = glob(self::PATH.'/*.mem.inc');
    $files = $files ? $files : array();
    $ts = time();
    $ttl = 24 * 60 * 60;
    foreach ($files as $file) {
      if (!file_exists($file)) continue ;
      $fts = filemtime($file);
      if (($ts - $fts) > $ttl) {
        //        error_log(system('ls -lah '.$file));
        unlink($file);
      }
    }
    /*      $cmd = 'find '.self::PATH.' -name "*.mem.inc" -mtime +1 -exec rm {} \;';
     system($cmd);*/
  }

}