<?php

try {

  //  sleep(1);

  $key = $req->hasHeader('x-item') ? $req->getHeader('x-item') : $req->item;

  $item = Item::getInstanceByKey($key);

  $position = $dbo->fetchValue('SELECT MAX(position)'
                               .' FROM '.T('resource')
                               .' WHERE model_id='.$item->getModelId()
                               .  ' AND record_id='.$item->getId());
  $position += 1;

  if ($req->hasHeader('x-file-name')) {
    
    $filename = File::NameClean($req->getHeader('x-file-name'));
    
    if (!Folder::exists(Resource::PATH)) {
      Folder::create(Resource::PATH);
    }
    
    $filepath = Resource::PATH.'/'.$filename;
    
    $in = $req->getStream();
    if (!is_resource($in)) {
      throw new Exception('invalid input');
    }
    $out = fopen($filepath, 'w');
    if (!is_resource($out)) {
      throw new Exception('invalid output');
    }
    while (!feof($in)) {
      fwrite($out, fread($in, 8192));
    }
    fclose($out);
    fclose($in);
    
    $filesize = $req->getHeader('x-file-size');

  }
  elseif ($req->hasAttribute('filepath', true)) {
    
    $filepath = trim($req->filepath, '/');

    if (!file_exists($filepath)) {
      throw new Exception('invalid file ['.$filepath.']');
    }
    
    $file = new File($req->filepath);
    $filepath = $file->getName();
    $filename = $file->getBaseName();
    $filesize = $file->getSize();

  }
  
  $type = 0;
  if (File::isImage($filename)) {
    $type = Resource::TYPE_IMAGE;
  }

  $attributes = array('model_id' => $item->getModelId(),
                      'record_id' => $item->getId(),
                      'name' => $filename,
                      'path' => $filepath,
                      'position' => $position,
                      'size' => $filesize,
                      'type' => $type);

  $resource = Resource::getInstance();
  $resource->commit($attributes);

  if (file_exists($filepath)) {
    $log->trace('resource file removed : '.
		'use $item->onResourceCreate($resource) '.
		'to move the file elsewhere');
    unlink($filepath);
  }

}
catch (Exception $e) { 
  if (strlen($filepath) > 6 && 
      file_exists($filepath) && 
      strpos($filepath, Resource::PATH) !== false) {
    unlink($filepath);
  }
  $log->error($e);
  $err->add($e); 
}
