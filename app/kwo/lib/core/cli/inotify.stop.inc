<?php

try {

  if (Inotify::init() === false) {
    return;
  }

  $pid = Inotify::getPid();
  if ($pid < 1) {
    return;
  }

  posix_kill($pid, SIGTERM);

  @unlink(Inotify::PID_FILE);

}
catch (Exception $e) { $err->add($e); }
