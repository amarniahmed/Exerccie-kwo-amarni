<?php

try {

  $file = File::getInstance($req->file_path, 'w');
  $file->write($req->content);
  $res->sendRedirect('/back/core/file.content.edit', 
                 array('file_path' => $file->getName()));

}
catch (Exception $e) { $log->error($e); }



