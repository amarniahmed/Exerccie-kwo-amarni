<?php

class Scheduler
{
  const PATH = 'var/log/scheduler';
  const SEPARATOR = "\n--\n";

  public static function clean() {
    if (strlen(self::PATH) < 9 || !is_dir(self::PATH)) return ;
    $files = glob(self::PATH.'/*.*');
    $files = $files ?: array();
    foreach ($files as $file) {
      if (!file_exists($file)) continue ;
      unlink($file);
    }
  }

}