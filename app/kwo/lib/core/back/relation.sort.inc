<?php

try { 

  $item = Item::getInstanceByKey($req->item);
  $filters = array();
  
  $table = $dbo->getTable($req->table);

  $rel_models = SH::asSet($req->models);
  $rel_model = $rel_models[0];
  if (T($rel_model) == $req->table) {
    $filters['id'] = 0;
  }
  elseif (count($rel_models) == 1 && $table->hasField($rel_model.'_id')) {
    $filters[$item->getModel().'_id'] = $item->getId();
    $filters[$rel_model.'_id'] = 0;
  }
  else {
    $filters[$item->getModel().'_id'] = $item->getId();
    $filters['model_id'] = 0;
    $filters['record_id'] = 0;
  }

  $opts = array('limit' => 1);

  $position = 1;
  foreach (SH::asSet($req->relations) as $key) {
    $relation = Item::getInstanceByKey($key);
    if (isset($filters['id'])) {
      $filters['id'] = $relation->getId();
    }
    elseif (isset($filters['model_id'])) {
      $filters['model_id'] = $relation->getModelId();
      $filters['record_id'] = $relation->getId();
    }
    else {
      $filters[$rel_model.'_id'] = $relation->getId();
    }
    $opts['filters'] = $filters;
    $dbo->update($table, array('position' => $position), $opts); 
    $position++;
  }

}
catch (Exception $e) { $err->add($e); }
