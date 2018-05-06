<?php

try {

  $currency = Currency::getInstance(array('code' => $req->code,
                                          'status' => 1));

  if ($currency->code === P('currency')) {
    $req->getCookie()->removeAttribute('currency_id');
  }
  else {
    $req->getCookie()->currency_id = $currency->getId();
  }

  $req->track('currency ('.$currency->code.')');

}
catch (Exception $e) {  }