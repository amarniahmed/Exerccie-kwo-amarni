<?php

try {

  $limit = $req->limit >= 1 && $req->limit <= 30 ? $req->limit : 5;

  $filters = array();
  if ($req->forum_id >= 1) {
    $filters['forum_id'] = (int) $req->forum_id;
  } 

  $opts = array('filters' => $filters,
                'include' => array('forum', 'user'),
                'last' => $limit,
                'metas' => array('child', 'display'));

  $ctx->topics = Topic::collect($opts);
  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }