<?php

try { 

  $values = array();

  if ($req->node_id >= 1) {
    $node = Node::getInstance((int) $req->node_id,
                              array('include' => 'tree')); 
    $tree = $node->getTree();
    if (!$tree->hasFlag(Tree::FLAG_PUBLIC)) {
      throw new Exception('tree is not public');
    }
    $values = $tree->asColumnHash($node->getId()); 
  }
  elseif ($req->tree_id) {
    $tree = Tree::getInstance((int) $req->tree_id);
    if (!$tree->hasFlag(Tree::FLAG_PUBLIC)) {
      throw new Exception('tree is not public');
    }
    $values = $tree->asColumnHash();
  }

  $ctx->values = $values;

}
catch (Exception $e) { $err->add($e); }
