<?php

try { 

  $item = Item::getInstance(MODEL, $req->id);

  $displays = $dbo->query('SELECT DATE_FORMAT(created_on, "%Y-%m") AS month_key,'
                          .     ' SUM(counter) AS n'
                          .' FROM '.Item::TABLE_LOG
                          .' WHERE model_id='.$item->getModelId()
                          .'   AND record_id='.$item->getId()
                          .'   AND type='.Item::LOG_TYPE_IMPRESSION
                          .' GROUP BY month_key'
                          .' ORDER BY month_key DESC')->asHash();
  
  $rs = $dbo->query('SELECT DATE_FORMAT(created_on, "%Y-%m") AS month_key,'
                    .     ' type,'
                    .     ' COUNT(*) AS n'
                    .' FROM '.Interaction::TABLE
                    .' WHERE model_id='.$item->getModelId()
                    .'   AND record_id='.$item->getId()
                    .' GROUP BY month_key, type'
                    .' ORDER BY month_key DESC, type');
  
  $months = array();
  foreach ($rs as $row) {
    $key = $row['month_key'];
    if (isset($displays[$key])) {
      $months[$key]['affichage'] = $displays[$key];
    }
    $months[$key][$row['type']] = $row['n'];
  }

  $ctx->months = $months;
  $ctx->model = MODEL;
  $ctx->model_id = $item->getModelId();
  $ctx->record_id = $item->getId();

  $res->useTemplate('core:interaction.timeline');

}
catch (Exception $e) { $err->add($e); }