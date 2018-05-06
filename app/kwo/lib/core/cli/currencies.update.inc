<?php

try {

  $currencies = Currency::collect(array('filters' => array('status' => 1)));
  
  $url = 'http://finance.yahoo.com/d/quotes.csv?e=.csv&f=sl1d1t1&s=EURZZZ=X';
  
  $failure_count = 0;
  
  foreach ($currencies as $currency) {
    $raw = file_get_contents(str_replace('ZZZ', $currency->code, $url));
    list(, $rate) = explode(',', $raw);
    $rate = (float) $rate;
    if ($rate <= 0.01) {
      $failure_count++;
      continue;
    }
    //    echo $currency->code.' '.$currency->rate.' -> '.$rate."\n";
    $currency->commit(array('rate' => $rate));
  }
  
  if ($failure_count > 50) {
    $log->trace('currency update failure');
  }

}
catch (Exception $e) { $log->error($e); }