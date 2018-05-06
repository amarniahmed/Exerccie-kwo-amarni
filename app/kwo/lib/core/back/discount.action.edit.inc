<?php

try {

  $discount = Discount::getInstance((int) $req->id);

  $conditions = $dbo->query('SELECT *'
                            .' FROM '.Discount::TABLE_CONDITION
                            .' WHERE discount_id='.$discount->getId());
  if ($conditions->count() < 1) {
    return ;
  }

  $action = $discount->getAction();

  $target = null;
  $targets = null;
  $threshold = null;
  $threshold_type = null;

  if ($discount->isCart()) {
    $targets = array(Model::id('order').':amount' => l('le montant de la commande'),
                     Model::id('order').':shipping' => l('les frais de port de la commande'));
    foreach ($conditions as $condition) {
      if ($condition['model_id'] == Model::id('order')) continue ;
      $key = $condition['model_id'].':'.$condition['attribute'].':'.$condition['value'];
      $value = Model::name($condition['model_id']).' '.$condition['attribute'].'='.$condition['value'];
      $targets[$key] = $value;
    }
    $threshold = $action['threshold'];
    if (($threshold - floor($threshold)) == 0) {
      $threshold = (int) $threshold;
      if ($threshold <= 0) $threshold = '';
    }
    $threshold_types = array('' => '--',
                             'amount' => l('montant'),
                             'quantity' => l('quantité'));
    $target = $action['model_id'].':'.$action['attribute'].':'.$action['value'];
  }
  else {

  }

  $ctx->discount = $discount;
  $ctx->model_id = $action['model_id'];
  $ctx->is_order = $ctx->model_id === Model::id('order');
  $ctx->action_id = (int) $action['id'];
  $ctx->target = $target;
  $ctx->targets = $targets;
  $ctx->threshold = $threshold;
  $ctx->threshold_types = $threshold_types;
  $ctx->threshold_type = $action['threshold_type'];
  $ctx->rate = $action['rate'];

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }