<?php

$mode = 0777;

if (!file_exists(DOC_PATH) && strlen(DOC_PATH) > 2) {
  mkdir(DOC_PATH, $mode, true);
}

/*if (!file_exists(USR_PATH)) {
  if (strlen(USR_PATH) <= 2) {
    throw new Exception('bad USR_PATH');
  }
  mkdir(USR_PATH, $mode, true);
  $level1 = array_merge(range(1, 9), range('a', 'z'));
  foreach ($level1 as $c1) {
    @mkdir(USR_PATH.'/'.$c1, $mode, true);
  }
}*/

if (!file_exists(Logger::PATH) && strlen(Logger::PATH) > 5) {
  mkdir(Logger::PATH, $mode, true);
  touch(Logger::ERR_FILE);
  touch(Logger::URL_FILE);
}

Session::build();

if (!file_exists(Cache::PATH) && strlen(Cache::PATH) > 5) {
  mkdir(Cache::PATH, $mode, true);
  $letters = array_merge(range(0, 9), range('a', 'z'), range('A', 'Z'));
  foreach ($letters as $l) {
    mkdir(Cache::PATH.'/'.$l, $mode, true);
  }
}

if (!file_exists(TMP_PATH) && strlen(TMP_PATH) > 5) {
  mkdir(TMP_PATH, $mode, true);
}

if (!file_exists(STORAGE_PATH.'/.htaccess')) {
  file_put_contents(STORAGE_PATH.'/.htaccess',
                    'order deny,allow'."\n".'deny  from all');
}
