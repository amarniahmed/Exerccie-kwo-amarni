<?php

try {

  //$req->debug();
  //  return ;

  $file_path = $req->file_path;

  $img = Img::getInstance($file_path);

  if ($req->operation === 'rotate') { 
    $img->rotate($req->angle);
    return ;
  }
  
  if ($req->operation === 'resize') {
    if ($req->width < 1 && $req->height < 1) return ;
    if ($req->width < 1 || $req->height < 1) {
      $ratio = $img->getWidth() / $img->getHeight();
    }
    $img->setSize($req->width > 0 ? $req->width : $req->height * $ratio,
                  $req->height > 0 ? $req->height : $req->width / $ratio);
    return ;
  }

  if ($req->operation === 'crop') {
    $x = $req->x;
    $y = $req->y;
    $width = $req->width;
    $height = $req->height;
    if ($req->ratio < 1) {
      $x = ceil($x / $req->ratio);
      $y = ceil($y / $req->ratio);
      $width = ceil($width / $req->ratio);
      $height = ceil($height / $req->ratio);
    }
    $img->crop($x, $y, $width, $height);
    return ;
  }

}
catch (Exception $e) { $err->add($e); }
