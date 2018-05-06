<?php

try { 

  //$req->debug();

  $tree = Item::getInstance(MODEL, $req->id);

  $node_model = TreeManager::nodeModel($tree);

  $ctx->columns = TreeManager::columns($tree, $ctx->getBranchAttribute($node_model.'_id'));
  $ctx->node_model = $node_model;
  $ctx->tree_model = $tree->getModel();
  $ctx->tree_id = $tree->getId();

  $res->resetTemplates();
  $res->useTemplates('core:columns');

}
catch (Exception $e) { $err->add($e); }

