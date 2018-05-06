<?php

try {

  if (empty($req->name)) {
    throw new Exception('name missing');
  }
  
  $script_path = LIB_PATH.'/'.CORE.'/cli/static/'.$req->name.'.inc';

  if (!file_exists($script_path)) {
    throw new Exception('file missing');
  }

  include $script_path;

}
catch (Exception $e) { $log->error($e); }