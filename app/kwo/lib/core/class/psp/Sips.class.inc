<?php

/*

utilisée pour
- Mercanet (BNP)

Numéro de carte à utiliser pour tester : 4974934125497800

Cryptogramme de test :
Cryptogramme  cvv_response_code
600	4D
640	4D
650	50
653	53
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

class Sips {

  const DEV_MERCHANT_ID = '014022286611111';
  const OPT_PATH = 'lib/core/opt/psp/sips';

  private static function config() {
    $config = P('shop.psp');
    $config = $config['sips'];
    return $config;
  }

  public static function request($payment) {
    $amount = $payment->amount;
    $method = $payment->getMethod();
    $order = $payment->getOrder();
    $options = SH::asHash($method->options);

    $bin_path = self::OPT_PATH.'/request';
    $file_path = self::OPT_PATH.'/pathfile';

    if (!is_readable($file_path) || !is_executable($bin_path)) {
      S('log')->error('sips api, invalid file rights', __METHOD__);
      throw new Exception('bad rights');
    }

    $parameters = self::config();

    $merchant_id = $parameters['mode'] == 'test' ? self::DEV_MERCHANT_ID : $parameters['merchant_id'];

    $url = H::url('/payment');

    $amount = round($amount * 100);

    $language = $order->getUser()->locale == Locale::FR ? 'fr' : 'en';

    $h = array('merchant_id' => $merchant_id,
               'merchant_country' => 'fr',
               'amount' => $amount,
               'currency_code' => '978',
               'pathfile' => $file_path,
               'normal_return_url' => $url.'.response/-/item/'.$order->getKey(),
               'cancel_return_url' => $url.'.cancel/-/item/'.$order->getKey(),
               'automatic_response_url' => $url.'.callback/-/psp/sips',
               'language' => $language,
               'payment_means' => $options['NAME'] ? $options['NAME'].',2' : 'CB,2,VISA,2,MASTERCARD,2',
               'header_flag' => 'yes',
               'order_id' => $order->getId(). 'x' . $payment->getId());

    $param = '';
    foreach ($h as $key => $value) {
      $param .= ' '.$key.'='.$value;
    }

    $result = exec($bin_path.' '.$param);
    $result = trim($result, '!');
    if ($options['NAME']) {
      $result .= H::trigger('$(this).previous().down(\'input[type=image]\').click()');
    }

    list($code, $error, $message) = explode('!', $result);

    return $message;
  }

  public static function callback() {

    $req = S('req');

    $keys = array('code', 'error', 'merchant_id', 'merchant_country', 'amount',
                  'transaction_id', 'payment_means', 'transmission_date','payment_time',
                  'payment_date', 'response_code', 'payment_certificate',
                  'authorisation_id', 'currency_code', 'card_number', 'cvv_flag',
                  'cvv_response_code', 'bank_response_code', 'complementary_code',
                  'complementary_info', 'return_context', 'caddie', 'receipt_complement',
                  'merchant_language', 'language', 'customer_id', 'order_id', 'customer_email',
                  'customer_ip_address', 'capture_day', 'capture_mode', 'data',
                  //--
                  'order_validity', 'transaction_condition', 'statement_reference', 'card_validity',
                  'score value', 'score_color', 'score_info', 'score_threshold',
                  'score_profile', 'upan', 'threed_ls_code', 'threed_relegation_code');

    $result = exec(self::OPT_PATH.'/response'
                   .' '.'pathfile='.self::OPT_PATH.'/pathfile'
                   .' '.'message='.$req->DATA);

    if ($result[0] != '!') {
      throw new Exception('bad DATA parameter');
    }

    $result = trim($result, '!');

    $values = explode('!', $result);

    if (count($values) != count($keys)) {
      $values = array_pad($values, count($keys), '');
    }

    $h = array_combine($keys, $values);
    list ($order_id, $payment_id) = explode('x', $h['order_id']);

    if ($h === false) {
      throw new Exception('bad DATA parameter');
    }

    $order = Order::getInstance((int) $order_id);
    if ($order->isPaid()) {
      throw new Exception('order ('.$order->getId().') already paid [status='.$order->status.']');
    }

    $payment = Payment::getInstance((int) $payment_id);
    if ($payment->isReceived()) {
      throw new Exception('payment ('.$payment->getId().') already received [order='.$order->getId().']');
    }

    if ($h['code'] != 0) {
      S('log')->debug($h);
      throw new Exception('bad code ('.$h['code'].')');
    }

    $attrs = array('code' => $h['code'],
                   'status' => Payment::STATUS_RECEIVED,
                   'datas' => array('payment_transaction_id' => $h['transaction_id'],
                                    'payment_comment' => join('!', $h)));
    $payment->commit($attrs);
    $payment->sendLetter();

    $method = $payment->getMethod();
    if ($payment->isLast() || $method->hasFlag(Method::FLAG_INSTALLEMENT)) {
      $order->setStatus(Order::STATUS_PAID);
    }
    else {
      $order->setStatus(Order::STATUS_PARTLY_PAID);
    }
  }

  public static function response() {}

}