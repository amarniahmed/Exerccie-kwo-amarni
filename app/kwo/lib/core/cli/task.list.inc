<?php

try {

  if ($req->errors == 'yes') {
    Task::show(Task::STATUS_FAILED);
  }
  elseif ($req->last == 'yes') {
    Task::show(Task::STATUS_COMPLETED, 10);
  }
  else {
    Task::show();
  }

}
catch (Exception $e) { $err->add($e); }
