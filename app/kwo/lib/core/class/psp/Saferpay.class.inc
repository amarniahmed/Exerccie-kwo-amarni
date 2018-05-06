<?php

/***
  $parameter['shop.psp'] = array('saferpay' => array('login'       => 'e99867001',
                                                     'password'    => 'XAjc3Kna',
                                                     'account_id'  => '99867-94913159',
                                                     'url'         => array('success' => '',
                                                                            'fail'    => '')))


  Code bancaire : 97000001
  Fournit avec chaque montant une réponse d’autorisation positive.

  Code bancaire :  97000002
  AMOUNT=21nn, génère une réponse RESULT="65" et la valeur « nn » transmise correspond au code de refus dans AUTHRESULT.
  AMOUNT=99nn fournit une réponse positive avec « nn » secondes de retard. À partir de 30 secondes, une erreur de Timeout se produit.
  Tous les autres montants fournissent une réponse positive.

***/

class Saferpay {
  const PROVIDER_URL = 'https://www.saferpay.com/user/setup.asp';

  const METHOD_MASTERCARD = 1;
  const METHOD_VISA = 2;
  const METHOD_AMERICAN_EXPRESS = 3;
  const METHOD_DINNER_CLUB = 4;
  const METHOD_JCB = 5;
  const METHOD_TEST = 6;
  const METHOD_LASER = 7;
  const METHOD_BONUS = 8;
  const METHOD_POSTFINANCE_EFINANCE = 9;
  const METHOD_POSTFINANCE_CARD = 10;
  const METHOD_MAESTRO = 11;
  const METHOD_MYONE = 12;
  const METHOD_PRELEVEMENT = 13;
  const METHOD_FACTURE = 14;
  const METHOD_VIREMENT_EXPRESS = 15;
  const METHOD_PAYPAL = 16;
  const METHOD_GIROPAY = 17;
  const METHOD_IDEAL = 18;
  const METHOD_CLICKANDBUY = 19;
  const METHOD_HOMEBANKING = 20;
  const METHOD_MPASS = 21;

  private static function config() {
    $config = P('shop.psp');
    $config = $config['saferpay'];
    return $config;
  }

  public static function request($payment) {
    $amount = $payment->amount;
    $method = $payment->getMethod();
    $order = $payment->getOrder();

    $options = SH::asHash($method->options);
    $config = self::config();

    $user = $order->getUser();
    $customer = $order->getCustomer();

    /****/

    $link = self::createPaymentLink($payment);
    $html = '<form method="post" action="'.$link.'">'
          .'<div>'.c(l('step.method')).'</div>'
          .'<input class="btn" type="submit" value="'.c(l('payment')).'" />'
          .'</form>';

    return $html;
  }

  public static function callback() {
    $req = S('req');
    $data = urldecode($req->DATA);
    $signature = $req->SIGNATURE;
    $response = self::parseResponse($data);

    $order_id = (int)$response['orderid'];
    S('log')->debug(array($order_id, $data, $signature));

    $order = Order::getInstance((int) $order_id);
    if ($order->isPaid()) {
      throw new Exception('order ('.$order->getId().') already paid [status='.$order->status.']');
    }

    $payment = $order->getPayment();
    if ($payment->isReceived()) {
      throw new Exception('payment ('.$payment->getId().') already received [order='.$order->getId().']');
    }

    $confirm = self::verifPayConfirm($data, $signature);
    S('log')->debug($confirm);
    if (!$confirm['is_error']) {
      $status = Payment::STATUS_RECEIVED;
    }
    else {
      $status = Payment::STATUS_ERROR;
    }

    $attrs = array('status'      => $status,
                   'description' => $confirm['content'],
                   'datas'       => $data);

    $payment->commit($attrs);
    $payment->sendLetter();
    $method = $payment->getMethod();
    if ($payment->isLast()) {
      $order->setStatus(Order::STATUS_PAID);
    }
    else if ($confirm['is_error']) {
      $order->setStatus(Order::STATUS_ERROR);
    }
    else {
      $order->setStatus(Order::STATUS_PARTLY_PAID);
    }
  }

  public static function response() {}

  /***/

  private static function generateKeys() {
    $config = self::config();
    $client_path = self::getClientPath();
    $config_path = self::getConfigPath();
    $params = array('p' => $config_path,
                    'r' => self::PROVIDER_URL,
                    'u' => $config['login'],
                    'w' => $config['password']);

    exec('java -jar "'.$client_path.'" -conf '.self::hashToShellParams($params));
  }

  public static function createPaymentLink($payment, $saferpay_methods=array(self::METHOD_TEST), $user_profile=array()) {
    $config = self::config();
    $client_path = self::getClientPath();
    $config_path = self::getConfigPath();
    $opts = array('encapse_value' => true);

    $order = $payment->getOrder();
    $purchases = $order->getPurchases();

    $success = $config['url']['success'] ?: '/payment.response';
    $back    = $config['url']['back']    ?: '/payment.cancel';
    $fail    = $config['url']['fail']    ?: '/payment.error';
    $notify  = $config['url']['notify']  ?: '/payment.callback';

    $description = array();
    foreach ($purchases as $purchase) {
      $description[] = c($purchase->name);
    }
    $description = substr(implode(', ', $description), 0, 50);

    $amount = str_replace('.', '', number_format($payment->amount, 2, '.', ''));
    $key = $order->getKey();
    $params = array('p' => $config_path,
                    'attributes' => array('amount'         => $amount,
                                          'currency'       => self::getCurrency(),
                                          'accountid'      => $config['account_id'],
                                          'orderid'        => $payment->order_id,
                                          'description'    => $description,
                                          'successlink'    => H::url($success.'/-/item/'.$key),
                                          'backlink'       => H::url($back.'/-/item/'.$key),
                                          'faillink'       => H::url($fail.'/-/item/'.$key),
                                          'notifyurl'      => H::url($notify.'/-/psp/saferpay/item/'.$key),

                                          'ccname'         => 'no', // Active le champ pour la saisie du nom du titulaire de la carte sur la Page de Paiement (yes/no)
                                          'paymentmethods' => implode(',', $saferpay_methods),
                                          ));

    if (!empty($user_profile)) {
      $profile = array('company', 'gender', 'firstname', 'lastname', 'street', 'zip', 'city', 'country', 'email', 'phone');
      foreach ($user_profile as $key => $value) {
        if (!in_array($key, $profile) || empty($value)) unset($user_profile[$key]);
      }
      $params['attributes'] = array_merge($params['attributes'], $user_profile);
    }

    return shell_exec('java -jar "'.$client_path.'" -payinit '.self::hashToShellParams($params, $opts));
  }

  public static function getTestPaymentMethods() {
    return array(self::METHOD_TEST);
  }
  
  public static function getDefaultPaymentMethods() {
    return array(self::METHOD_AMERICAN_EXPRESS,
                 self::METHOD_MASTERCARD,
                 self::METHOD_VISA);
  }

  private static function verifPayConfirm($data, $signature) {
    $client_path = self::getClientPath();
    $config_path = self::getConfigPath();

    $params = array('p' => $config_path,
                    'd' => urlencode($data),
                    's' => $signature);
    $opts = array('encapse_value' => true);
    $response = shell_exec('java -jar "'.$client_path.'" -payconfirm '.self::hashToShellParams($params, $opts));

    return self::parseStringResponse($response);
  }

  /***/

  private static function getClientPath() {
    return realpath(LIB_PATH.'/core/opt/psp/saferpay/Saferpay.jar');
  }

  private static function getConfigPath() {
    $config = self::config();
    $client_path = self::getClientPath();
    $path = dirname($client_path).'/config_'.$config['login'];

    if (!file_exists($path)) {
      mkdir($path, 0744, true);
      if (file_exists($path)) {
        self::generateKeys();
      }
      else {
        throw new Exception('Keys can\'t been generated !');
      }
    }
    return $path;
  }

  private static function getCurrency() {
    $currency_id = S('req')->getCookie()->currency_id;
    $currency = 'EUR';
    if ($currency_id >= 1) {
      try {
        $c = Currency::getInstance(array('id' => $currency_id,
                                         'status' => 1));
        $currency = $c->code;
      }
      catch (Exception $e) {}
    }
    return empty($currency) ? 'EUR' : $currency;
    // W::currencies();
  }

  private static function hashToShellParams($hash, $opts=array()) {
    $str = '';
    foreach ($hash as $key => $value) {
      $key = $opts['key_case'] == 'upper'
                               ? strtoupper($key)
                               : ($opts['key_case'] == 'lower' ? strtolower($key) : $key);

      if (strtolower($key) == 'attributes') {
        foreach ((array)$value as $k => $v) {
          if ($opts['encapse_value']) $v = '"'.htmlentities($v).'"';
          $str .= ' -a '.strtoupper($k).' '.$v;
        }
      }
      else {
        if ($opts['encapse_value']) $value = '"'.htmlentities($value).'"';
        $str .= ' -'.$key.' '.$value;
      }
    }
    return ltrim($str);
  }

  private static function xmlParamsToHash($dom_element) {
    $params = array();
    if (!is_null($dom_element->attributes)) {
      foreach ($dom_element->attributes as $name => $node) {
        if(!is_null($dom_element->attributes)) {
          $params[strtolower($name)] = $node->nodeValue;
        }
      }
    }
    return $params;
  }

  private static function parseStringResponse($str) {
    $value = strtolower($str);
    $is_error = strpos($value, 'fail') !== false || strpos($value, 'error') !== false;
    return array('content'  => $str,
                 'is_error' => $is_error);
  }

  private static function parseResponse($str) {
    $str = urldecode($str);
    $doc = new DOMDocument();
    $doc->validateOnParse = false;
    $doc->strictErrorChecking = false;
    $result = $doc->loadXML($str);
    $error = libxml_get_errors();

    if (!empty($error)) {
      S('log')->error($str);
      throw new Exception('response is not a valid XML: '.print_r($error, true));
    }

    $hash = self::xmlParamsToHash($doc->documentElement);

    return $hash;
  }

  private static $payement_methods = array(1  => 'MasterCard',
                                           2  => 'Visa',
                                           3  => 'American Express',
                                           4  => 'Diners Club',
                                           5  => 'JCB',
                                           6  => 'Saferpay Test Card',
                                           7  => 'Laser Card',
                                           8  => 'Bonus Card',
                                           9  => 'PostFinance E-Finance',
                                           10 => 'PostFinance Card',
                                           11 => 'Maestro International',
                                           12 => 'MyOne',
                                           13 => 'Prélèvement',
                                           14 => 'Facture',
                                           15 => 'Virement express',
                                           16 => 'PayPal',
                                           17 => 'giropay',
                                           18 => 'iDEAL',
                                           19 => 'ClickandBuy',
                                           20 => 'Homebanking AT (eps)',
                                           21 => 'Mpass'); //page 21

}

?>