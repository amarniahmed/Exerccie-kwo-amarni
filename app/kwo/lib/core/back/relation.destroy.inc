<?php

try { 

  $item = Item::getInstanceByKey($req->item);
  $ifield = $item->getModel().'_id';

  $relation = Item::getInstanceByKey($req->relation);
  $rfield = $relation->getModel().'_id';

  $row = array();
  $table = $dbo->getTable($req->table);
  if ($req->table == T($relation)) {
    $row['id'] = $relation->getId();
  }
  elseif (substr($table->getName(), -5) === '_item') {
    $row[$ifield] = $item->getId();
    $row['model_id'] = $relation->getModelId();
    $row['record_id'] = $relation->getId();
  }
  else {
    $row[$ifield] = $item->getId();
    $row[$rfield] = $relation->getId();
  }

  $item->onRelationDestroy($relation);
  
  $where = array();
  foreach ($row as $key => $value) {
    $where[] = $key.'='.$value;
  }
  $where = count($where) > 0 ? join(' AND ', $where) : 0;

  $dbo->exec('DELETE FROM '.$table->getName()
             .    ' WHERE '.$where
             .    ' LIMIT 1');

}
catch (Exception $e) { $err->add($e); }



/*  elseif ($table->hasField($field)) {
    $row[$item->getModel().'_id'] = $item->getId();
    $row[$field] = $relation->getId();
  }
  else {
    $row[$item->getModel().'_id'] = $item->getId();
    $row['model_id'] = $relation->getModelId();
    $row['record_id'] = $relation->getId();
  } */