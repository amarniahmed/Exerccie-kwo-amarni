<?php

try {

  $discount = Discount::getInstance((int) $req->id);

  $attribute = '';
  $value = '';
  $threshold = 0;
  $threshold_type = '';

  if ($discount->isCart()) {
    list ($model_id, $attribute, $value) = explode(':', $req->target);
    $threshold = $req->threshold;
    $threshold_type = $req->threshold_type;
    if ($model_id == Model::id('order')) {
      $threshold_type = '';
    }
    if (empty($threshold_type)) {
      $threshold = 0;
    }
    if (!empty($threshold_type) && empty($threshold)) {
      throw new Exception(c(l('la valeur de la limite est invalide')));
    }
  }
  else {
    $model_id = $dbo->fetchValue('SELECT model_id FROM '.Discount::TABLE_CONDITION
                                 .             ' WHERE discount_id='.$discount->getId()
                                 .             ' LIMIT 1');
  }

  $rate = $req->rate;
  if ($rate != 'calc') {
    $suffix = strpos($rate, '%') !== false ? '%' : '';
    $rate = str_replace(',', '.', $rate);
    $rate = preg_replace('/[^0-9\.]/', '', $rate);
    $rate = (float) $rate;
    if ($rate <= 0) {
      throw new Exception(c(l('le montant est invalide')));
    }
    $rate = (string) $rate;
    $rate .= $suffix;
  }

  $row = array('rate' => $rate,
               'model_id' => $model_id,
               'attribute' => $attribute,
               'discount_id' => $discount->getId(),
               'value' => $value,
               'threshold' => $threshold,
               'threshold_type' => $threshold_type);

  if ($req->action_id > 0) {
    $filters = array('id' => (int) $req->action_id);
    $opts = array('filters' => $filters,
                  'limit' => 1);
    $dbo->update(Discount::TABLE_ACTION, $row, $opts);
  }
  else {
    $dbo->insert(Discount::TABLE_ACTION, $row);
  }

}
catch (Exception $e) { $err->add($e); }