<?php

try {
  
  $fields = array('icon', 'title', 'description', 'rows', 'value');

  foreach ($fields as $field) {
    if (!$req->hasAttribute($field, true)) {
      continue;
    }
    $ctx->$field = h($req->getAttribute($field));
  }
  $res->useTemplate();

}
catch (Exception $e) { $log->error($e); }