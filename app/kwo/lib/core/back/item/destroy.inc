<?php

try {

  if ($req->hasAttribute('items', true) && strlen($req->items) > 0) {
    $items = explode(',', $req->items);
    $i = 0;
    foreach ($items as $id) {
      try {
        $id = (int) $id;
        if ($id < 1) continue ;
        $item = Item::getInstance(MODEL, $id);
        $item->destroy();
        $log->trace($item->getModel().'('.$id.') '.$item->name);
        $i++;
      }
      catch (Exception $e) {}
    }
    $res->sendMessage($i.' '.l('élément(s) supprimé(s)'));
    return ;
  }

  $item = Item::getInstance(MODEL, $req->id);

  $ctx->id = 0;
  $ctx->is_node = false;

  if (TreeManager::isNode($item)) {
    $ctx->ancestor_id = $item->ancestor_id;
    $ctx->is_node = true;
  }

  $item->destroy();

  if ($ctx->is_node === true) {
    $tree_key = TreeManager::treeModel($item).'_id';
    $set = $dbo->asSet('SELECT id'
                       .' FROM '.T(MODEL)
                       .' WHERE '.$tree_key.'='.$item->getAttribute($tree_key)
                       .  ' AND path BETWEEN :min: AND :max:'
                       .  ' AND id<>'.$item->id,
                       array(':min:' => $item->path,
                             ':max:' => $item->path.'zz'));
    $nodes = Item::collect($item->getModel(),
                           array('filters' => array('id' => $set)));
    foreach ($nodes as $node) {
      $node->destroy();
    }
  }

  if (Model::isProfile(MODEL) && $req->getBranchValue('user_id', true) >= 1) {
    $ctx->action = 'edit';
  }
  else {
    $ctx->action = 'home';
  }

  $res->sendMessage(c(l('l’opération a bien été réalisée')));

}
catch (Exception $e) { $err->add($e); }