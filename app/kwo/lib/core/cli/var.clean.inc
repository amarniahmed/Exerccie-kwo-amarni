<?php

try {

  Logger::clean();
  UploadManager::clean();
  Session::clean();
  Cache::clean();
  Scheduler::clean();
  TaskServer::clean();
  Inotify::clean();
  Repository::clean();

}
catch (Exception $e) { $err->add($e); }
