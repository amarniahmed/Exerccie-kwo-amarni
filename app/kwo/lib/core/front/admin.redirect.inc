<?php

try {
  
  $res->resetTemplates();
  
  $script = "window.top.location='/".Admin::url()."';";

  $html = H::elt('img', null, array('on' => array('load' => $script)));

  $res->write($html);

}
catch (Exception $e) { $err->add($e); }
