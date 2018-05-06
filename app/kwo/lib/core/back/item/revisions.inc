<?php

try {

  $item = Item::getInstance(MODEL, (int) $req->id);

  $rs = $dbo->query('SELECT *'
                    .' FROM '.Item::TABLE_REVISION
                    .' WHERE model_id='.$item->getModelId()
                    .  ' AND record_id='.$item->getId()
                    .' ORDER BY created_at DESC'
                    .' LIMIT 200');

  $rows = array();
  foreach ($rs as $row) {
    $row['attributes'] = unserialize($row['attributes']);
    $row['creator'] = Model::name($row['creator_model_id']);
    try {
      if ($row['creator_record_id'] > 0) {
        $creator = Item::getInstance((int) $row['creator_model_id'],
                                     (int) $row['creator_record_id']);
        $row['creator'] .= ' : '.$creator->getName();
      }
    }
    catch (Exception $e) { 
      $row['creator'] .= ' ('.$row['creator_record_id'].')';
      $log->error($e); 
    }
    $rows[] = $row;
  }

  $ctx->rows = $rows;

  $res->useTemplates('core:revisions');

}
catch (Exception $e) { $err->add($e); }
