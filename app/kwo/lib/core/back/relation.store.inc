<?php

try {
//  d($req);
  $table = $dbo->getTable($req->table);

  $filters = array();

  $item = Item::getInstanceByKey($req->item);
  $item_field = $item->getModel().'_id';
  $filters[$item_field] = $item->getId();

  if ($req->hasAttribute('relation', true)) {
    $relation = Item::getInstanceByKey($req->relation);
  }
  else {
    $relation = Item::getInstance($req->model_id, $req->record_id);
  }
  $relation_field = $relation->getModel().'_id';

  $models = SH::asSet($req->models);

  if (count($models) == 1 && $table->hasField($relation_field) && $relation->getModel() != $item->getModel()) {
    $filters[$relation_field] = $relation->getId();
  }
  else {
    $filters['model_id'] = $relation->getModelId();
    $filters['record_id'] = $relation->getId();
  }

  $row = $filters;

  $req->row = (array) $req->row;
  foreach ($req->row as $key => $value) {
    $row[$key] = $value;
  }
  
  if ($table->hasRow($filters)) {
    if (!is_array($req->row)) return ;
    $opts = array('filters' => $filters,
                  'limit' => 1);
    $dbo->update($table, $row, $opts); 
    $item->onRelationUpdate($relation);
  }
  else {
    if ($table->hasField('position')) {
      $position = $dbo->fetchValue('SELECT MAX(position)'
                                   .' FROM '.$table->getName()
                                   .' WHERE '.$item_field.'='.$item->getId());
      $row['position'] = $position + 1;
    }
    $dbo->insert($table, $row);
    $item->onRelationCreate($relation);
  }

}
catch (Exception $e) { $err->add($e); }
