<?php

try {

  $discount = Discount::getInstance((int) $req->id);

  $attribute = strtolower($req->attribute);
  $value = $req->value;
  $model_id = (int) $req->model_id;
  $condition_id = (int) $req->condition_id;

  $threshold = $req->threshold;
  $threshold_type = $req->threshold_type;
  if ($model_id == Model::id('order')) {
    $threshold_type = '';
  }
  if (empty($threshold_type)) {
    $threshold = 0;
  }

  if ($model_id == Model::id('order')) {
    $attrs = array('amount', 'quantity', 'coupon', 'user');
    if (!in_array($attribute, $attrs)) {
      throw new Exception(c(l('attribut invalide')));
    }
    if ($attribute == 'coupon') {
      $n = $dbo->fetchValue('SELECT COUNT(*) FROM '.Discount::TABLE_CONDITION
                            .             ' WHERE id<>'.$condition_id
                            .               ' AND attribute="'.$attribute.'"'
                            .               ' AND value=:coupon:',
                            array(':coupon:' => $value));
      if ($n >= 1) {
        throw new Exception(c(l('ce code coupon a déjà été utilisé')));
      }
    }
  }
  else {
    $attrs = array_merge(array('id', 'code', 'type', 'category', 'tag', 'mpn', 'sku'), (array) P('shop.discount.condition.attributes'));
    if (!in_array($attribute, $attrs)) {
      throw new Exception(c(l('attribut invalide')));
    }
  }

  $row = array('model_id' => $model_id,
               'attribute' => $attribute,
               'discount_id' => $discount->getId(),
               'value' => $value,
               'threshold' => $threshold,
               'threshold_type' => $threshold_type);

  if ($condition_id > 0) {
    $filters = array('id' => (int) $condition_id);
    $opts = array('filters' => $filters,
                  'limit' => 1);
    $dbo->update(Discount::TABLE_CONDITION, $row, $opts);
  }
  else {
    $dbo->insert(Discount::TABLE_CONDITION, $row);
  }

}
catch (Exception $e) {
  $log->error($e);
  $err->add($e);
}