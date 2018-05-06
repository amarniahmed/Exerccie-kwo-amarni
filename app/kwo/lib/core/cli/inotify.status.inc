<?php

try {

  if (Inotify::init() === false) {
    return;
  }

  if (Inotify::getPid() > 0) {
    echo 'inotify daemon is running ['.Inotify::getPid().']'."\n";
    return;
  }

  echo 'inotify daemon is not running'."\n";

}
catch (Exception $e) { $err->add($e); }
