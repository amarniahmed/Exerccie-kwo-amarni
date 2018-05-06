<?php

try {

  $opts = array('filters' => array('user_id' => $user->getId()),
                'order' => array('id' => 'desc'));

  $cagnotte = $user->getCredit();

  $credits = array();

  $credit_sum = 0;
  foreach (Credit::collect($opts) as $credit) {
    $credit_sum += $credit->amount;
    $credits[] = $credit;
    if ($credit_sum >= $cagnotte) break ;    
  }

  $ctx->credits = $credits;
  $ctx->cagnotte = $cagnotte;
  $ctx->credit_sum = $credit_sum;

  $res->setMenu('credits');
  $res->setTitle(l('account.credits'));


}
catch (Exception $e) { $err->add($e); }

