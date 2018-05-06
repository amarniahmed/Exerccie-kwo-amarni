<?php

try {
  
  if ($req->key != SH::crypt($req->model_id.$req->record_id)) {
    return ;
  }

  $opts = array('filters' => array('model_id' => $req->model_id,
                                   'record_id' => $req->record_id,
                                   'status' => 1),
                'last' => 1);
  
  if ($req->last_update >= 1) {
    $opts['filters']['|>|created_at'] = DH::toDb($req->last_update);
  }
  
  $comments = Comment::collect($opts);
  
  if ($comments !== false && $comments->numRows() >= 1) {
    /* TODO : refresh cat view */
  }

}
catch (Exception $e) { $err->add($e); }