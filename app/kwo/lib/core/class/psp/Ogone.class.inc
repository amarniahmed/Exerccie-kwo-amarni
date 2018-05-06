<?php

/*****

Passphrase SHA-IN  : Configuration > Tech Info > Data and origin verif
Passphrase SHA-OUT : Configuration > Tech Info > Transaction feedback
Set both to the same value.

http://domain/payment.callback/-/psp/balinea/status/ok
http://domain/payment.callback/-/psp/balinea/status/ko

VISA : 4111 1111 1111 1111
Visa 3-D Secure : 4000 0000 0000 0002
MasterCard : 5399 9999 9999 9999
American Express : 3741 1111 1111 111

Config example by sofiane

$parameter['shop.psp'] = array('ogone' => array('mode' => 'test',
                                                'params' => array('pspid' => 'kernix2012',
                                                                  'passphrase' => '8g6sge1Vajnxk+fvmyoA')));
*****/

class Ogone {

/*****
 *
en_US for English (default)
dk_DK for Danish
nl_NL for Dutch
nl_BE for Flemish
fr_FR for French
de_DE for German
it_IT for Italian
ja_JP for Japanese
no_NO for Norwegian
pl_PL for Polish
pt_PT for Portugese
es_ES for Spanish
se_SE for Swedish
tr_TR for Turkish

******/

  public static $languages = array(I18N::LOCALE_FR => 'fr_FR',
                                   I18N::LOCALE_EN => 'en_EN',
                                   I18N::LOCALE_ES => 'es_ES',
                                   I18N::LOCALE_IT => 'it_IT');
  /** config by sofiane **/
  private static function config() {
    $config = P('shop.psp');
    $config = $config['ogone'];
    return $config;
  }

  public static function request($payment) {   /** $item [ORDER] to $payment by sofiane **/
    $config = self::config();
    $params = $config['params'];
    $server_url = 'https://secure.ogone.com/ncol/'.$config['mode'].'/orderstandard'.($config['charset'] == 'utf8' ? '_utf8' :'').'.asp';

    $amount   = (int) (number_format($payment->amount, 2, '.', '') * 10 * 10);
    $order    = $payment->getOrder();
    $user     = $order->getUser();
    $customer = $order->getCustomer();
    $method   = $payment->getMethod();

    $url = H::url('/payment');
    $language = self::$languages[$user->getLocale()];
    $language = empty($language) ? 'fr_FR' : $language;
    $options  = SH::asHash($method->options);

    $response_url = $config['payment.response'] ?: '/payment.response';
    $cancel_url   = $config['payment.cancel'] ?: '/payment.cancel';
    $args = array('item' => $order->getKey());

    $hash = array('PSPID' => $params['pspid'],
                  'ORDERID' => $order->getId(). 'x' . $payment->getId(),
                  'AMOUNT' => $amount,
                  'CURRENCY' => 'EUR',
                  'LANGUAGE' => $language,
                  'CN' => substr($customer->last_name.' '.$customer->first_name, 0, 35),
                  'EMAIL' => substr($user->email, 0, 50),
                  'OWNERZIP' => substr($customer->postal_code, 0, 10),
                  'OWNERADDRESS' => substr($customer->address.' '.$customer->address_extra, 0, 35),
                  'OWNERTOWN' => substr($customer->city, 0, 25),
                  'OWNERCTY' => Country::code($customer->country_id),
                  'OWNERTELNO' => substr($customer->phone, 0, 30),
                  'COM' => 'ORDER#'.$order->getId(),
                  'HOMEURL' => substr($url, 0, 200),
                  'CATALOGURL' => substr($url, 0, 200),
                  'TP' => H::url(WEB_URL.'/templates/ogone.html'),
                  'ACCEPTURL' => H::url($response_url, $args),
                  'DECLINEURL' => H::url($cancel_url, $args),
                  'EXCEPTIONURL' => H::url($cancel_url, $args),
                  'CANCELURL' => H::url($cancel_url, $args));

    $protected = array('CN', 'OWNERZIP', 'OWNERADDRESS', 'OWNERTOWN', 'OWNERTELNO');
    foreach ($protected as $field) {
      $value = $hash[$field];
      $value = SH::stripAccents($value);
      $value = preg_replace('/[^[:alnum:]]/u', ' ', $value);
      $hash[$field] = $value;
    }

    $hash  = array_map('trim', array_merge($hash, $options));
    $datas = array_change_key_case($hash, CASE_UPPER);
    ksort($datas);

    $str = '';
    foreach ($datas as $key => $value) {
      $value = trim($value);
      if (empty($value)) continue ;
      $str .= $key . '=' . $value . $params['passphrase']; /** $config['passphrase'] by sofiane **/
    }

    $hash['SHASIGN'] = strtoupper(sha1($str, false));

    $form = Template::render('psp.ogone',
                             array('hash' => $hash,
                                   'server_url' => $server_url));
    return $form;
  }

  // Direct HTTP server-to-server request

  public static function callback() {
    /**
     * 24/01/2014: AECK
     * Modification du calcul de la clé SHA-OUT
     * Prise en compte des paramètres dont la valeur vaut '0'
     * Ordonnancement des paramètres selon l'ordre Ogone spécifique
     */
    $req = S('req');
    $req->debug();
    $config = self::config();
    $params = $config['params'];
    
    $hash = array_change_key_case($req->getAttributes(), CASE_UPPER);
    $new_ar = array();
    
    foreach (Ogone::$out_params as $key) {
      if (array_key_exists($key, $hash)) {
        $new_ar[$key] = $hash[$key];
      }
    }
    $hash = $new_ar;
    
    /** order_id x payment_id by sofiane **/
    list ($order_id, $payment_id) = explode('x', $hash['ORDERID']);
  
    if ($order_id < 1) {
      throw new Exception('invalid order_id ('.$order_id.')');
    }
  
    $str = '';
    foreach ($hash as $key => $value) {
      $value = trim($value);
      if ($value === '' || $key === 'SHASIGN' || !in_array($key, Ogone::$out_params)) continue ;
      $str .= $key . '=' . $value . $params['passphrase'];
    }

    /*$req = S('req');
$req->debug();
    $hash = array_change_key_case($req->getAttributes(), CASE_UPPER);
    ksort($hash);
    $config = self::config();
    $params = $config['params'];
    /** order_id x payment_id by sofiane *
    list ($order_id, $payment_id) = explode('x', $hash['ORDERID']);

    if ($order_id < 1) {
      throw new Exception('invalid order_id ('.$order_id.')');
    }

    $str = '';
    foreach ($hash as $key => $value) {
      $value = trim($value);
      if (empty($value) || $key === 'SHASIGN' || !in_array($key, self::$out_params)) continue ;
      $str .= $key . '=' . $value . $params['passphrase'];
    }*/

    $shasign = sha1($str, false);

    if (strtoupper($shasign) != strtoupper($hash['SHASIGN'])) {
      $req->debug(true);
      throw new Exception('invalid shasign');
    }

    $order = Order::getInstance($order_id);
    if ($order->isPaid()) {
      throw new Exception('order ('.$order->getId().') already paid [status='.$order->status.']');
    }
    /** Payment control by sofiane **/
    $payment = Payment::getInstance((int) $payment_id);
    if ($payment->isReceived()) {
      throw new Exception('payment ('.$payment->getId().') already received [order='.$order->getId().']');
    }

    $status = (int) $hash['STATUS'];

    $datas = '';
    foreach ($hash as $key => $value) {
      $datas .= $key.'='.$value."\n";
    }
    /** Payment status & commit control by sofiane **/
    if ($status == 5 || $status == 9) {
      $status = Payment::STATUS_RECEIVED;
    }
    else {
      $status = Payment::STATUS_ERROR;
    }
    
    $attrs = array('code'   => $hash['PAYID'],
                   'status' => $status,
                   'datas'  => $datas);
    $payment->commit($attrs);
    $payment->sendLetter();

    if ($payment->isLast()) {
      $order->setStatus(Order::STATUS_PAID);
    }
    else {
      $order->setStatus(Order::STATUS_PARTLY_PAID);
    }
  }


  public static function response() {}

  public static $out_params = array('AAVADDRESS', 'AAVCHECK', 'AAVZIP', 'ACCEPTANCE',
                                    'ALIAS', 'AMOUNT', 'BIN', 'BRAND', 'CARDNO', 'CCCTY', 'CN',
                                    'COMPLUS', 'CREATION_STATUS', 'CURRENCY', 'CVCCHECK',
                                    'DCC_COMMPERCENTAGE', 'DCC_CONVAMOUNT', 'DCC_CONVCCY',
                                    'DCC_EXCHRATE', 'DCC_EXCHRATESOURCE', 'DCC_EXCHRATETS',
                                    'DCC_INDICATOR', 'DCC_MARGINPERCENTAGE', 'DCC_VALIDHOURS',
                                    'DIGESTCARDNO', 'ECI', 'ED', 'ENCCARDNO', 'FXAMOUNT',
                                    'FXCURRENCY', 'IP', 'IPCTY',
                                    'NBREMAILUSAGE', 'NBRIPUSAGE', 'NBRIPUSAGE_ALLTX',
                                    'NBRUSAGE', 'NCERROR', 'NCERRORCARDNO', 'NCERRORCN',
                                    'NCERRORCVC', 'NCERRORED', 'ORDERID', 'PAYID', 'PM',
                                    'SCO_CATEGORY', 'SCORING', 'SHASIGN', 'STATUS',
                                    'SUBSCRIPTION_ID', 'TRXDATE', 'VC');

}
