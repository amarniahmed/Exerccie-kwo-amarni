<?php 

/*
Numéro de carte à utiliser pour tester : 4974934125497800

Cryptogramme de test :
Cryptogramme  cvv_response_code
600     4D
640     4D
650     50
653     53
655 55
Autres  4E

Valeur  Signification
4E  Numéro de contrôle incorrect
4D  Numéro de contrôle correct
50  Numéro de contrôle non traité
53  Le numéro de contrôle est absent de la demande d’autorisation
55  La banque de l’internaute n’est pas certifiée, le contrôle n’a pu être effectué.
NO  Pas de cryptogramme sur la carte.
*/



class Mercanet {
  
  const DEV_MERCHANT_ID = '082584341411111';
  const OPT_PATH = 'lib/shop/opt/psp/mercanet';

  public static function request($item) {

    $bin_path = self::OPT_PATH.'/request';
    $file_path = self::OPT_PATH.'/pathfile.txt';

    if (!is_readable($file_path) || !is_executable($bin_path)) {
      S('log')->error('mercanet api, invalid file rights', __METHOD__);
      throw new Exception('bad rights');
    }

    $parameters = P('shop.params');

    $merchant_id = P('shop.mode') == 'test' ? self::DEV_MERCHANT_ID : $parameters['merchant_id'];
    
    $url = H::url('/shop/payment');

    $amount = round($item->getAmount() * 100);

    $language = $item->getUser()->locale == I18N::LOCALE_FR ? 'fr' : 'en';

    $hash = array('merchant_id' => $merchant_id,
                  'merchant_country' => 'fr',
                  'amount' => $amount,
                  'currency_code' => '978',
                  'pathfile' => $file_path,
                  'normal_return_url' => $url.'.response/-/id/'.$item->getId(),
                  'cancel_return_url' => $url.'.cancel',
                  'automatic_response_url' => $url.'.callback/-/psp/mercanet',
                  'language' => $language,
                  'payment_means' => 'CB,2,VISA,2,MASTERCARD,2',
                  'header_flag' => 'yes',
                  'order_id' => $item->getId());

    $param = '';
    foreach ($hash as $key => $value) {
      $param .= ' '.$key.'='.$value;
    }

    $result = exec($bin_path.' '.$param);
    $result = trim($result, '!');

    list($code, $error, $message) = explode('!', $result);

    return $message;
  }
  
  public static function callback() {

    $req = S('req');

    $keys = array('code','error','merchant_id','merchant_country','amount',
                  'transaction_id','payment_means','transmission_date','payment_time',
                  'payment_date', 'response_code', 'payment_certificate', 
                  'authorisation_id', 'currency_code', 'card_number', 'cvv_flag', 
                  'cvv_response_code', 'bank_response_code', 'complementary_code', 
                  'complementary_info', 'return_context', 'caddie', 'receipt_complement', 
                  'merchant_language', 'language', 'customer_id', 'order_id', 'customer_email',
                  'customer_ip_address', 'capture_day', 'capture_mode', 'data');
    
    $result = exec(self::OPT_PATH.'/response'
                   .' '.'pathfile='.self::OPT_PATH.'/pathfile.txt'
                   .' '.'message='.$req->DATA);

    if ($result[0] != '!') {
      throw new Exception('bad DATA parameter');
    }

    $result = trim($result, '!');

    $values = explode('!', $result);

    if (count($values) != count($keys)) {
      $values = array_pad($values, count($keys), '');
    }

    $hash = array_combine($keys, $values);

    if ($hash === false) {
      throw new Exception('bad DATA parameter');
    }

    $order = Order::getInstance((int) $hash['order_id']);
    if ($order->isPaid()) {
      throw new Exception('order ('.$order->getId().') already paid [status='.$order->status.']');
    }
    
    if ($hash['code'] != 0) {
      S('log')->debug($hash);
      throw new Exception('bad code ('.$hash['code'].')');
    }

    $order->commit(array('status' => Order::STATUS_PAID,
                         'payment_transaction_id' => $hash['transaction_id'],
                         'payment_comment' => join('!', $hash)));
  }

  public static function response() {}

}
