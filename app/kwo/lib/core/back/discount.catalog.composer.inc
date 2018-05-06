<?php

try {

  $discount = Discount::getInstance($req->id);

  $rs = $dbo->query('SELECT * FROM '.Discount::TABLE_CONDITION
                    .      ' WHERE discount_id='.$discount->getId());
  $conditions = array();
  $i = 1;

  foreach ($rs as $row) {
    $msg = $i ==  1 ? l('si le produit').' ' : ' '.l('et si le produit').' ';
    $msg .= ' [<i>'.Model::name($row['model_id']).'.'.$row['attribute'];
    $msg .= '=';
    $msg .= $row['value'].'</i>] ';
    $conditions[$row['id']] = $msg;
    $i++;
  }

  $ctx->action = $discount->action;
  $ctx->conditions = $conditions;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }