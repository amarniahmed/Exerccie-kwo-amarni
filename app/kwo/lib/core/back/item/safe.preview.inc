<?php

$res->resetTemplates();

try {

  $message = ActiveRecord::getInstance(MODEL, $req->id);
  
  $content = $item->content;
  if (!preg_match('/<body/i',$content)) {
    $content = '<html><body>'.$content.'</body></html>';
  }

  $res->write($content);

}
catch (Exception $e) { $err->add($e); }
