<?php

try {

//  $codes = explode(',', $req->strings);

  $session = $req->isTraced();

  $rows = $dbo->query('SELECT model_id, record_id'
                      .' FROM '.Item::TABLE_TRACER
                      .' WHERE session='.$session
                      .' ORDER BY model_id'); 
  $dbo->exec('DELETE FROM '.Item::TABLE_TRACER.' WHERE session='.$session);

  $models = array();
  foreach ($rows as $row) {
    $model_id = $row['model_id'];
    $record_id = $row['record_id'];
    if (!isset($models[$model_id])) $models[$model_id] = array();
    if (isset($models[$model_id][$record_id])) continue ;
    $models[$model_id][$record_id] = Item::getInstance($model_id, $record_id);
  }

  $ctx->models = $models;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
