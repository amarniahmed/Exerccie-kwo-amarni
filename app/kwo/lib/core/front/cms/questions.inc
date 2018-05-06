<?php

try { 

  $values = array();

  if ($req->top) { 
    list ($key, $value) = explode(':', $req->top);
    $values[$key] = $value;
  }
  
  if ($req->id >= 1) {
    $faq = Faq::getInstance((int) $req->id);
    if (!$faq->isOnline()) {
      throw new MaliciousException();
    }
    foreach ($faq->getComponents() as $component) {
      $values[$component->getId()] = $component->name; 
    }
  }

  if ($req->bottom) { 
    list ($key, $value) = explode(':', $req->bottom);
    $values[$key] = $value;
  }

  $ctx->values = $values;

}
catch (Exception $e) { $err->add($e); }
