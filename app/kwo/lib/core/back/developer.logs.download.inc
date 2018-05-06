<?php

try { 

  $t = Logger::archive(true); 

  $file = File::getInstance(Logger::ERR_FILE.'_'.$t.'.gz'); 

  $res->sendData($file);

}
catch (Exception $e) { $err->add($e); }
