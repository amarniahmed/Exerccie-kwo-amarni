<?php

try {

  //  $req->debug();

  $item = Item::getInstanceByKey($req->item);
  $item_field = $item->getModel().'_id';
  $filters = array($item_field => $item->getId());

  $skip = array('associated_at', $item_field, 'position');

  $table = $dbo->getTable($req->table);

  $relation = Item::getInstanceByKey($req->relation);
  $relation_field = $relation->getModel().'_id';
  if ($table->hasField($relation_field)) {
    $skip[] = $relation_field;
    $filters += array($relation_field => $relation->getId());
  }
  else {
    $skip[] = 'model_id';
    $skip[] = 'record_id';
    $filters += array('model_id' => $relation->getModelId(),
                      'record_id' => $relation->getId());
  }
  
  $row = $dbo->fetchHash('SELECT *'
                         .' FROM '.$table->getName()
                         .' WHERE '.AH::asSql($filters, false)
                         .' LIMIT 1');
  //  $log->debug($row);
  $fields = array();

  foreach ($table->getFields() as $field) {
    if (in_array($field, $skip)) continue ;
    $fields[] = $field;
  }
  //  $log->debug($fields);

  $elts = array();
  foreach ($fields as $field) {
    $elts[$field] = array('name' => $field,
                          'label' => $field,
                          'type' => 'text',
                          'value' => $row[$field]);
  }

  if (method_exists($item, 'asRelationEditHash')) {
    $elts = $item->asRelationEditHash($elts, $relation);
  }

//  $ctx->item = $item;
  $ctx->relation = $relation;
//  $ctx->row = $row;
  $ctx->elts = $elts;
//  $ctx->fields = $fields;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
