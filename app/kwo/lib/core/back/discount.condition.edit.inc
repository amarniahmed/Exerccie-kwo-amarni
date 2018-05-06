<?php

try {

  $discount = Discount::getInstance($req->id);

  $threshold_types = array('' => '--',
                           'amount' => l('montant'),
                           'quantity' => l('quantité'));

  $row = $dbo->fetchHash('SELECT * FROM '.Discount::TABLE_CONDITION
                         .      ' WHERE id='.intval($req->condition_id)
                         .        ' AND discount_id='.$discount->getId());

  $discount_models = P('shop.discount.models');
  $models = array();
  if ($discount->isCart()) {
    $models = array(Model::id('order') => l('panier'));
    foreach ($discount_models as $model) {
      if (!Model::exists($model)) {
        S('log')->error('discount model ['.$model.'] unknown');
        continue ;
      }
      $models[Model::id($model)] = Model::name($model);
    }
  }
  elseif (!empty($row)) {
    $models = array($row['model_id'] => Model::name($row['model_id']));
  }
  else {
    foreach ($discount_models as $model) {
      $models[Model::id($model)] = Model::name($model);
    }
  }

  $threshold = $row['threshold'];
  if (($threshold - floor($threshold)) == 0) {
    $threshold = (int) $threshold;
  }

  $ctx->discount = $discount;
  $ctx->condition_id = (int) $row['id'];
  $ctx->model_id = $row['model_id'] ?: Model::id('order');
  $ctx->is_order = $ctx->model_id == Model::id('order');
  $ctx->models = $models;
  $ctx->attribute = $row['attribute'];
  $ctx->value = $row['value'];
  $ctx->threshold_type = $row['threshold_type'];
  $ctx->threshold_types = $threshold_types;
  $ctx->threshold = $threshold > 0 ? $threshold : '';

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }