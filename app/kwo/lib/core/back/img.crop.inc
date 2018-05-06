<?php

try {
  $file = new Img($req->file_path);
  $ret = $file->crop($req->x1,$req->y1,$req->x2,$req->y2);
  $res->write("file.view('".$file->getName()."');");
}
catch (Exception $e) { $err->add($e); }

