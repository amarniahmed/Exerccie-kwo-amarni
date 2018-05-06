<?php

 /*
  * pensez à définir la directive "extension_dir" dans
  * /etc/php-cli.ini si inotify.so n'est pas déjà lancé
  *
  */

class Inotify {

  const PID_FILE = 'var/run/inotify.watcher.pid';
  private static $pid = 0;
  
  public static function getPid() {
    if (self::$pid > 0) {
      return self::$pid;
    }

    $log = S('log');
    if (!is_file(Inotify::PID_FILE)) {
      return false;
    }
    
    $pid = (int) @file_get_contents(Inotify::PID_FILE);
    
    if ($pid === false) {
      $log->error('error occurred while reading pid file ['.Inotify::PID_FILE.']', __METHOD__);
      return false;
    }
    
    if (!is_dir('/proc/'.$pid)) {
      $log->error('[/proc/'.$pid.'] directory not found', __METHOD__);
      return false;
    }
    
    $cmd_line = @file_get_contents('/proc/'.$pid.'/cmdline');
    if ($cmd_line === false) {
      $log->error('error occurred while reading [/proc/'.$pid.'/cmdline] file', __METHOD__);
      return false;
    }

    if (!(strpos($cmd_line, 'inotify.start') >= 0)) {
      $log->error('bad process', __METHOD__);
      return false;
    }
    
    self::$pid = $pid;

    return $pid;
  }

  public static function init($load_extension=false) {
    
    $daemons = (array) P('app.daemons');
    if (!in_array('inotify.watcher', $daemons) && S('req')->force != 'yes') {
      return false;
    }

    if (!$load_extension) {
      return true;
    }

    if (!function_exists('inotify_init')) {
      $ret = dl('inotify.so');
      if ($ret === false) {
        S('log')->error('inotify.so extension failed to load', __METHOD__);
        return false;
      }
    }
    return true;
  }

  public static function trigger() {
    touch(Task::PATH);
  }
  
  public static function clean() {
    if (file_exists(self::PID_FILE)) {
      unlink(self::PID_FILE);
    }
  }

}