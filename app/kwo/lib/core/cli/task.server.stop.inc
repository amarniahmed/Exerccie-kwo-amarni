<?php

try {

//  $pid = file_get_contents(TaskServer::PID_FILE); 
//  posix_kill($pid, SIGTERM);

  // kill -s SIGTERM  `cat var/run/task.server.pid`

  TaskServer::call('quit');

}
catch (Exception $e) { $err->add($e); }
