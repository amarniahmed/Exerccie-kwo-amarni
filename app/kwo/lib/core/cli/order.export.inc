<?php

try {

  $doc = new DOMDocument('1.0', 'UTF-8');
  $doc->xmlStandalone = true;
  $doc->formatOutput = true;
  $xml = $doc->createElement('orders');
  $xml->setAttribute('date', DH::now());
  $xml = $doc->appendChild($xml);

  $rs = $dbo->query('SELECT *, o.id AS order_id, p.country_id AS country'
                    .' FROM '.T('order').' AS o,'
                    .     ' '.T('user').' AS u,'
                    .     ' '.T('person').' AS p'
                    .' WHERE o.status>='.Order::STATUS_PROCESSED
                    .  ' AND (o.flags & '.Order::FLAG_EXPORTED.') < 1'
                    .  ' AND o.user_id=u.id'
                    .  ' AND u.id=p.user_id');
  
  $attributes = array('order_id', 'invoice_id', 'amount_ttc', 'amount_ht',
                      'tax', 'label', 'paid_at', 'user_id', 'payment_transaction_id',
                      'first_name', 'last_name', 'email', 'address',
                      'postal_code', 'country', 'phone');
  
  $order = array();

  foreach ($rs as $row) {
    $node = $doc->createElement('order');
    foreach ($attributes as $attribute) {
      $value = $row[$attribute];
      switch ($attribute) {
      case 'amount_ttc': $value = number_format($row['total_amount'], 
                                                2, '.', ''); break;
      case 'amount_ht': $value = number_format($row['total_amount'] - $row['tax_amount'], 
                                               2, '.', ''); break;   
      case 'tax': $value = '20'; break;
      case 'country': $value = Country::code($value); break;
      case 'label': $value = 'abonnement scene-juridique'; break;
      }
      $node->appendChild($doc->createElement($attribute, $value));
    }
    $xml->appendChild($node);
    $orders[] = (int) $row['order_id'];
  }

  echo $doc->saveXML();

  foreach ($orders as $id) {
    $order = Order::getInstance($id);
    //$order->setFlag(Order::FLAG_EXPORTED);
  }

}
catch (Exception $e) { $err->add($e); }
