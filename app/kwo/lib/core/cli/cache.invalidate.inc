<?php

try {

  $parameter = Parameter::getInstance('cache.state');
  $parameter->commit(array('value' => P('cache.state') + 1));

}
catch (Exception $e) { $err->add($e); }
