<?php

try {
  
  $item = Item::getInstance(MODEL, $req->id);

  $opts = array('filters' => array('model_id' => $item->getModelId(),
                                   'record_id' => $item->getId()),
                'include' => array('user'),
                'reverse' => true,
                'metas' => array('created_at'));
  
  $comments = Comment::collect($opts);

  $ctx->comments = $comments;

  $res->useTemplates('core:item.comments');

}
catch (Exception $e) { $err->add($e); }
