<?php

$res->resetTemplates();

try {

  $body = Body::getInstance($req->id);
  
  $content = $body->content_html;
  if (!preg_match('/<body/i',$content)) {
    $content = '<html><body>'.$content.'</body></html>';
  }

  $res->write($content);

}
catch (Exception $e) { $err->add($e); }

