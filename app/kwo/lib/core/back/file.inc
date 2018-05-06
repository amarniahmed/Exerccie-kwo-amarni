<?php

try {

  $file = File::isImage($req->path)
        ? Img::getInstance($req->path)
        : File::getInstance($req->path);

  $details['nom'] = wordwrap(basename($file->getBaseName()), 22, '<br/>', 1);
  $details['chemin'] = '/'.wordwrap($file->getPath(), 22, '<br/>', 1);
  $details['date'] = date('d/m/Y G:i', $file->getCtime());
  $details['taille'] = NH::asFileSize($file->getSize());
  if (File::isImage($file)) {
    $details['dim'] = join(' x ', 
                           array($file->getWidth(), $file->getHeight()));
  }

  $ctx->details = $details;
  $ctx->file = $file;

  $res->setTitle($file->getFolder()->getHtmlPath());
  $res->useTemplate();

}
catch (Exception $e) { return $err->add($e); }
