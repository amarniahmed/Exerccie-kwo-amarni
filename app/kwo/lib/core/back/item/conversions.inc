<?php

try {

  $limit = 5000;
  $offset = 0;

  $item = Item::getInstance(MODEL, $req->id);

  $medium = Item::asMedium($item);
  $source = Item::asSource($item);

  $rs = $dbo->query('SELECT c.*, v.medium, v.source, v.term, v.campaign'
                    .' FROM '.T('conversion').' AS c,'
                    .     ' '.Visit::TABLE.' AS v'
                    .' WHERE c.visit_id=v.id'
                    .  ' AND v.medium='.$medium
                    .  ' AND v.source='.$source
                    .' ORDER BY c.id DESC'
                    .' LIMIT '.$limit
                    .' OFFSET '.$offset);

  $rows = array();

  $models = array();

  foreach ($rs as $row) {
    $models[$row['model_id']] = (array) $models[$row['model_id']];
    $models[$row['model_id']]['count']++;
    $models[$row['model_id']]['worth'] += $row['worth'];
    $rows[] = $row;
  }

  $ctx->rows = $rows;

  $res->useTemplate('core:item.conversions');

}
catch (Exception $e) { $err->add($e); }
