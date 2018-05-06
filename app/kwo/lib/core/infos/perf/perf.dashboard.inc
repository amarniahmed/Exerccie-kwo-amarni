<?php

try {

  $limit = 50;

  $where = '';
  if ($req->medium > 0) {
    $where .= ' AND v.medium='.intval($req->medium);
  }
  if ($req->model_id > 0) {
    $where .= ' AND c.model_id='.intval($req->model_id);
  }
  if ($req->model_id > 0 && $req->record_id > 0) {
    $where .= ' AND c.record_id='.intval($req->record_id);
  }
  if (!empty($req->source)) {
    $where .= ' AND v.source='.Tracker::str2id($req->source, Tracker::EXPR_SOURCE, false);
  }
  if (!empty($req->term)) {
    $where .= ' AND v.term='.Tracker::str2id($req->term, Tracker::EXPR_TERM, false);
  }
  if (!empty($req->campaign)) {
    $where .= ' AND v.campaign='.Tracker::str2id($req->campaign, Tracker::EXPR_CAMPAIGN, false);
  }
  if ($req->user_id > 0) {
    $where .= ' AND c.user_id='.intval($req->user_id);
  }

  $rs = $dbo->query('SELECT SQL_CALC_FOUND_ROWS c.*, v.medium, v.source, v.term, v.campaign'
                    .' FROM '.T('conversion').' as c,'
                    .     ' '.Visit::TABLE.' as v'
                    .' WHERE c.visit_id=v.id'.$where
                    .' ORDER BY created_at DESC'
                    .' LIMIT '.$limit.' OFFSET '.intval($req->offset * $limit)); 

  $strings = array();
  foreach ($rs as $row) {
    $strings[$row['source']]++;
    $strings[$row['term']]++;
    $strings[$row['campaign']]++;
  }

  $strings = array_keys($strings);
  unset($strings[0]);

  $set = $dbo->asSet('SELECT DISTINCT model_id FROM '.T('conversion'));
  $types = array();
  foreach ($set as $model_id) {
    $types[$model_id] = Model::name($model_id);
  }

  if (in_array(Model::id('goal'), $set)) {
    
  }

  $ctx->types = $types;
  $ctx->mediums = Tracker::$mediums;

  $ctx->strings = $dbo->asHash('SELECT id, name'
                               .' FROM '.Tracker::TABLE_EXPRESSION
                               .' WHERE id IN ([id])',
                               array('[id]' => $strings)); 

  $ctx->rs = $rs; 

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
