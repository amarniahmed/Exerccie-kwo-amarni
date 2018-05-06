<?php

try {

  if (!$user->hasFile($req->image)) {
    throw new Exception('file unknown ['.$req->image.']');
  }

  $width = P('avatar.width') > 1 ? P('avatar.width') : 36;
  $height = P('avatar.height') > 1 ? P('avatar.height') : 36;

  $user->getFile($req->image)
    ->getImg()
    ->setSize($width, $height)
    ->rename('_a.jpg');

  $user->setFlag(User::FLAG_IMAGE);

}
catch (Exception $e) {
  $log->error($e);
  $err->add(l('error.upload.image'));
}
