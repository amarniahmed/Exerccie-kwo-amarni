<?php

try {

  $discount = Discount::getInstance($req->id);

  $rs = $discount->getConditions();

  $conditions = array();
  $i = 1;

  foreach ($rs as $row) {
    $msg = $i ==  1 ? l('si').' ' : ' '.l('et si').' ';
    if ($row['model_id'] == Model::id('order')) {
      if ($row['attribute'] == 'coupon') {
        $msg .= l('le <strong>panier</strong> dispose du coupon <strong>((value))</strong>', array('value' => $row['value']));
      }
      elseif ($row['attribute'] == 'user') {
        $user_attr = strpos($row['value'], '@') > 1 ? 'email' : 'id';
        $msg .= l('le <strong>panier</strong> a pour client le user dont l’<strong>((attribute)) est ((value))</strong>',
                  array('attribute' => $user_attr,
                        'value' => $row['value']));
      }
      elseif ($row['attribute'] == 'quantity') {
        $msg .= l('le <strong>panier</strong> contient au moins <strong>((value)) produits</strong>', array('value' => $row['value']));
      }
      elseif ($row['attribute'] == 'amount') {
        $msg .= l('le <strong>montant du panier</strong> est supérieur ou égal à <strong>((value)) euros</strong>', array('value' => $row['value']));
      }
      $msg .= ' '.$value;
    }
    elseif ($discount->isCart()) {
      $msg .= l('le panier contient');
      if ($row['threshold'] > 0) {
        $threshold = $row['threshold'];
        if (($threshold - floor($threshold)) == 0) {
          $threshold = (int) $threshold;
        }
        if ($row['threshold_type'] === 'quantity') {
          $msg .= ' '.l('au moins ((threshold)) produit', array('threshold' => $threshold));
          $msg .= $row['threshold'] > 1 ? 's ' : ' ';
        }
        else {
          $msg .= ' '.l('pour au moins ((threshold)) euro(s) de produits', array('threshold' => $threshold)).' ';
        }
      }
      else {
        $msg .= ' '.l('au moins un produit').' ';
      }
      $msg .= ' [<i>';
      $msg .= Model::name($row['model_id']);
      $msg .= '.'.$row['attribute'].'='.$row['value'].'</i>]';
    }
    else {
      $msg .= l('le produit <strong>((model))</strong> dispose de l’attribut <strong>((attribute))</strong> = <strong>((value))</strong>',
                array('model' => Model::name($row['model_id']),
                      'attribute' => $row['attribute'],
                      'value' => $row['value']));
    }
    $conditions[$row['id']] = $msg;
    $i++;
  }

  $row = $discount->getAction();
  $threshold = $row['threshold'];
  if (($threshold - floor($threshold)) == 0) {
    $threshold = (int) $threshold;
  }
  $action = '';
  if (!empty($row)) {
    $discount_rate = $row['rate'];
    if (strpos($row['rate'], '%') === false) {
      $discount_rate .= ' euros';
    }
    //$action .= 'réduction de <strong>'.$discount_rate.'</strong> sur ';
    $action .= l('réduction de <strong>((discount))</strong> sur', array('discount' => $discount_rate)).' ';
    /*$action .= 'réduction de <strong>';
    $action .= $row['rate'];
    if (strpos($row['rate'], '%') === false) {
      $action .= ' euros';
    }
    $action .= '</strong> sur ';*/

    if ($row['model_id'] == Model::id('order')) {
      $action .= $row['attribute'] === 'shipping' ? l('les frais de port') : l('le montant');
      $action .= ' '.l('du panier');
      if ($row['threshold'] > 0) {
        $action .= ' '.l('dans une limite de').' '.$threshold;
        $action .= ' '.($row['threshold_type'] === 'quantity' ? l('produit(s)') : l('euro(s)'));
      }
    }
    else {
      $action .= ' '.l('le produit');
      if (!empty($row['attribute'])) {
        $action .= ' [<i>'.Model::name($row['model_id']);
        $action .= '.'.$row['attribute'].'='.$row['value'].'</i>]';
        if ($row['threshold'] > 0) {
          $action .= ' '.l('dans une limite de').' '.$threshold;
          $action .= ' '.($row['threshold_type'] === 'quantity' ? l('produit(s)') : l('euro(s)'));
        }
      }
    }
  }

  $ctx->discount = $discount;
  $ctx->action = $action;
  $ctx->conditions = $conditions;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }