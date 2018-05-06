<?php

try {

  $journal = 'JVT';

  $orders = $dbo->query('SELECT *'
                        .' FROM '.T('order')
                        .' WHERE status='.Order::STATUS_PROCESSED
                        .' ORDER BY id DESC'
                        .' LIMIT 3');

  $opts = array('columns' => array('journal', 'code_compta', 'date', 
                                   'num_piece', 'nom_client', 'credit', 'debit'));
  
  $csv = CsvWriter::getInstance($opts);

  if ($orders === false || $orders->numRows() < 1) return ;
  
  while ($order = $orders->getNext()) {
    $date = DH::asHash($order['completed_at']);
    $user = User::getInstance($order['user_id']);
    $line = array('journal' => $journal, 
                  'code' => '',
                  'date' => $date['year'].$date['month'].$date['day'],
                  'num' => 'FA-'.str_pad($order['id'], 7, '0', STR_PAD_LEFT), 
                  'client' => $user->name,
                  'credit' => 0, 
                  'debit' => 0);
    $line['code'] = '4119'.str_pad($user->id, 7, '0', STR_PAD_LEFT);
    $line['credit'] = $order['total_amount'];
    $line['debit'] = 0;
    $csv->append($line);
    if ($order['discount_amount'] > 0) {
      $line['code'] = '709XXX';
      $line['credit'] = $order['discount_amount'];
      $line['debit'] = 0;
      $csv->append($line);
    }
    if ($order['handling_amount'] > 0 || $order['shipping_amount'] > 0) {
      $amount = $order['handling_amount'] + $order['shipping_amount'];
      $line['code'] = '7085';
      $line['credit'] = 0;
      $line['debit'] = $amount;
      $csv->append($line);
      $line['code'] = '445712';
      $line['credit'] = 0;
      $line['debit'] = round($amount * Order::SHIPPING_TAX / 100, 2);
      $csv->append($line);
    }
    $purchases = $dbo->query('SELECT *'
                             .' FROM '.T('purchase')
                             .' WHERE order_id='.$order['id']);
    foreach ($purchases as $purchase) {
      $amount = $purchase['price'] * $purchase['quantity'];
      $line['code'] = '701SARF';
      $line['credit'] = 0;
      $line['debit'] = round($amount, 2);
      $csv->append($line);
      $line['code'] = '445712';
      $line['credit'] = 0;
      $line['debit'] = round($amount * $purchase['tax'] / 100, 2);
      $csv->append($line);
    }
  }

}
catch (Exception $e) { $err->add($e); }

