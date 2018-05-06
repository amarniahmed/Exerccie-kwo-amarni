<?php

try {

  $img = Img::getInstance($req->file_path);
  $img->setSize($req->width, $req->height);
  $ctx->file_path = $img->getName();

}
catch (Exception $e) { $err->add($e); }

