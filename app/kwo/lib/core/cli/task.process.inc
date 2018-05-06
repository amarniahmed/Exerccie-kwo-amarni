<?php

try {

  if ($req->id > 0) {
    $task = Task::getInstance((int) $req->id);
    $task->process();
    return ;
  }

  $rs = $dbo->query('SELECT id FROM '.T('task')
                    .       ' WHERE status='.Task::STATUS_PENDING);
  foreach ($rs as $row) {
    $task = Task::getInstance((int) $row['id']);
    $task->process();
  }
  
}
catch (Exception $e) { $err->add($e); }
