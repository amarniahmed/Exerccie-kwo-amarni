<?php

try {

  $img = Img::getInstance($req->file_path);
  $img->rotate($req->angle);
  $ctx->file_path = $img->getName();

}
catch (Exception $e) { $err->add($e); }