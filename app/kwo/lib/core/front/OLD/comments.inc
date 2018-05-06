<?php

try {

  $item = Item::getInstanceByKey($req->item);

  $filters = array('model_id' => $item->getModelId(),
                   'record_id' => $item->getId(),
                   'status' => 1);

  $opts = array('filters' => $filters,
                'reverse' => true,
                'limit' => 10,
                'offset' => (int) $req->offset,
                'include' => array('user'),
                'metas' => array('created_at'));

  $comments = Comment::collect($opts);

  $ctx->item = $item;
  $ctx->comments = $comments;
  $ctx->offset = (int) $req->offset;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }