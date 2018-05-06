<?php

try {

  $folders = array(DOC_PATH, ETC_PATH, LIB_PATH, PUB_PATH, VAR_PATH, WEB_PATH);

  foreach ($folders as $folder) {
    $s = Shell::du($folder);
    echo $folder . ': ' . $s."\n";
  }

}
catch (Exception $e) { $err->add($e); }
