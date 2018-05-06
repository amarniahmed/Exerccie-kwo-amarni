<?php

try {

  $code = (int) $_SERVER['REDIRECT_STATUS'];
  
//  error_log($_SERVER['SCRIPT_URI']);
  $err->add('error '.$code);
  
  $parts = (array) explode('.', $_SERVER['SCRIPT_URI']);
  $extension = array_pop($parts);
  $exclude = array('jpg', 'jpeg', 'htm', 'html', 'js',
                   'htm', 'gif', 'png', 'css', 'ico', 'swf');
  if (in_array(strtolower($extension), $exclude)) { 
    $res->resetTemplates();
    $res->write('error 404');
  }
  else {
    $ctx->code = (int) $code;
    $res->useTemplate();
  }

  if (false) {
    $str = $code.' : '.$_SERVER['SCRIPT_URI'];
    if (!empty($_SERVER['QUERY_STRING'])) {
      $str .= '?'.$_SERVER['QUERY_STRING'];
    }
    $log->trace($str);
  }

}
catch (Exception $e) { 
  $str = '[Exception] '.$code.' : '.$_SERVER['SCRIPT_URI'];
  if (!empty($_SERVER['QUERY_STRING'])) {
    $str .= '?'.$_SERVER['QUERY_STRING'];
  }
  $log->trace($str);
  $err->add($e); 
}

