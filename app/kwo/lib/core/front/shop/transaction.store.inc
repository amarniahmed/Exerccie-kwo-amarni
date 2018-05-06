<?php

try { 
  
  $transaction = Transaction::getInstance();
  
  $amount = str_replace(array(' ', 'EUR', ','),
                        array('', '', '.'),
                        $req->amount);

  $amount = (float) $amount;
  if ($amount < 1) {
    throw new Exception('montant invalide');
  }
  
  $transaction->commit(array('customer' => SH::clean($req->customer),
                             'description' => SH::clean($req->description),
                             'amount' => $amount));

  $res->write(Payment::request($transaction));

}
catch (Exception $e) { $res->write($e->getMessage()); }