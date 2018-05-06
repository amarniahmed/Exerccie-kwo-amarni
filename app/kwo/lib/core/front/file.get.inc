<?php

try {

  $path = $req->path;
  
  if ($req->hasAttribute('item')) {
    $item = Item::getInstanceByKey($req->item);
    $item->track(Interaction::TYPE_DOWNLOAD);
    if (empty($path)) {
      $path = $item->file;
    }
  }
  
  $file = File::getInstance($path, 'r');

  $req->track($file->getPath(), 'download');

  if ($req->hasHeader('range') && $req->getMethod() === 'GET') {
    list (,$range) = explode('=', $req->getHeader('range'));
    list ($from, $to) = explode('-', $range);
    $req->sendPartialContent($file, $from, $to);
  }
  else {
    $res->sendData($file);
  }

}
catch (Exception $e) { 
  $req->track($path.' / download', Tracker::CATEGORY_ERROR);
  $err->add($e); 
}
