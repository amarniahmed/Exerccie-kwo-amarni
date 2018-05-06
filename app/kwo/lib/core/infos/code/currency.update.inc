<?php

try {

  file_put_contents('tmp/devise.csv',trim(file_get_contents('http://www.banque-france.fr/fr/poli_mone/taux/telnomot/2.csv')));

  $h = fopen('tmp/devise.csv','r');
  while ($buffer_arr = fgetcsv($h,0,';')) {
    if (++$i == 6) {
      $currency = array_flip($buffer_arr);
    }
    $lastline_arr = $buffer_arr;
  }
  fclose($h);
  
  foreach (Currency::collect('Currency') as $current) {
    if (!empty($lastline_arr[$currency[$current->code]]) && $lastline_arr[$currency[$current->code]] != 'ND') {
      $current->rate = $lastline_arr[$currency[$current->code]];
    }
  }

  unlink('tmp/devise.csv');

} catch (Exception $e) { $err->add($e); }