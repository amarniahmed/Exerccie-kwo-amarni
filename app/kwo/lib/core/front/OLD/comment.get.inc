<?php

$res->setContentType(Response::TYPE_JSON);

try {
  
  /*  if ($req->key != SH::crypt($req->model_id.$req->record_id)) {
    return ;
  }*/

  $opts = array('filters' => array('model_id' => $req->model_id,
                                   'record_id' => $req->record_id,
                                   'status' => 1),
                'reverse' => true,
                'limit' => 2,
                'offset' => (int) $req->offset,
                'include' => 'user');
  
  $ctx->comments = Comment::collect($opts);

  if ($ctx->comments->numRows() < 1) { 
    throw new Exception(); 
  }

}
catch (Exception $e) { $err->add($e); }