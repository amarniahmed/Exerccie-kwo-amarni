<?php

try {

  if ($req->hasAttribute('code')) {
    $args = array('code' => $req->code);
  }
  elseif ($req->hasAttribute('id')) {
    $args = (int) $req->id;
  }
  else {
    throw new Exception('args missing');
  }
  
  $form = Form::getInstance($args);

  $form->track();
 
  $res->write($form->asHtml($res->getLocale()));
  
}
catch (Exception $e) { $err->add($e); }