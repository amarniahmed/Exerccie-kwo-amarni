<?php

try {

  if (Inotify::init(true) === false) {
    return;
  }

  if (Inotify::getPid() > 0) {
    $log->warn('inotify already running');
    return;
  }

  $pid = Shell::daemonize();

  if ($pid > 0) {
    $ret = @file_put_contents(Inotify::PID_FILE, $pid);
    if ($ret === false) {
      $log->error('pid file writing failed');
    }
    return ;
  }

  $handle = inotify_init();
  if ($handle === false) {
    $log->error('inotify initialisation failed');
    return ;
  }

  $watch_descriptor = inotify_add_watch($handle, Task::PATH, IN_ATTRIB);
  if ($watch_descriptor === false) {
    $log->error('add watch on ['.Task::PATH.'] failed');
    return ;
  }

  while (true) {
    inotify_read($handle);
    $output_lines = array();
    exec(Shell::PHP_BIN.' '.Platform::BOOTSTRAP.' task.process', $output_lines, $ret);
    if ($ret !== Platform::EXIT_STATUS_SUCCESS) {
      $log->error('inotify : task.process failed');
    }
  }

  $ret = inotify_rm_watch($handle, $watch_descriptor);
  if ($ret === false) {
    $log->error('remove watch on ['.Task::PATH.'] failed');
    return ;
  }

  fclose($handle);

}
catch (Exception $e) { $err->add($e); }