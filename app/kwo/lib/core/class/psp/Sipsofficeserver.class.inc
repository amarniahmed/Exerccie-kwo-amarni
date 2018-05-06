<?php

/**

Démmarage de Sip Office server:

var/sips/server/script/launch.sh

Arret:
var/sips/examples/php/StopServer.php (arrete le watchdog et le serveur)


$parameter['shop.psp'] = array('sipsofficeserver' => array('mode' => 'test',
                                                           'payment.response' => '/payment.response.sipsofficeserver',
                                                           'payment.cancel' => '/payment.cancel',
                                                           'params' => array('office' => array('host' => '127.0.0.1',
                                                                                               'service.port' => 7180,
                                                                                               'command.port' => 7181),
                                                                             'merchant_id' => '011223344553333',
                                                                             'merchant_country' => 'fr')));

*/


class Sipsofficeserver {

  private static function config() {
    $config = (array) P('shop.psp');
    $config = (array) $config['sipsofficeserver'];
    return $config;
  }

  private static function getServiceSocket(&$errno, &$errstr) {
    $config = self::config();
    $params = $config['params']['office'];
    $socket = fsockopen($params['host'], $params['service.port'], $errno, $errstr, 4);

    if (!$socket) {
      throw new Exception('Socket error: '.$errno.' ['.$errstr.']');
    }
    return $socket;
  }

  private static function getCommandSocket(&$errno, &$errstr) {
    $config = self::config();
    $params = $config['params']['office'];
    $socket = fsockopen($params['host'], $params['command.port'], $errno, $errstr, 4);

    if (!$socket) {
      throw new Exception('Socket error: '.$errno.' ['.$errstr.']');
    }
    return $socket;
  }

  private static function xmlParamsToHash($dom_element) {
    $params = array();
    if (!is_null($dom_element->attributes)) {
      foreach ($dom_element->attributes as $name => $node) {
        if(!is_null($dom_element->attributes)) {
          $params[$name] = $node->nodeValue;
        }
      }
    }
    return $params;
  }

  private static function loadXml($socket, $xml) {
    $ret = fwrite($socket, $xml);
    $sresp = fread($socket, 2048);
    $sresp = str_replace('&', '&amp;', $sresp);

    $doc = new DOMDocument();
    $doc->validateOnParse = false;
    $doc->strictErrorChecking = false;
    $result = $doc->loadXML($sresp);
    $error = libxml_get_errors();

    if (!empty($error)) {
      S('log')->error($sresp);
      throw new Exception('response is not a valid XML: '.print_r($error, true));
    }

    return $doc;
  }

  public static function request($payment) {
    $config = self::config();
    $params = $config['params'];

    $param['payment.response'] = $param['payment.response'] ?: '/payment.response.sipsofficeserver';
    $param['payment.cancel'] = $param['payment.cancel'] ?: '/payment.cancel';

    $html = '<form id="form_sipsofficeserver" action="/payment.request.sipsofficeserver" method="post">';
    $html .= '<fieldset>';
    $html .= '<legend><input type="radio" name="mode" checked="checked" value="carte" /> Paiement par Carte</legend>';
    $html .= '<span>Numéro de carte</span>';
    $html .= '<input type="text" name="cb"><br />';
    $html .= '<span>Cryptogramme visuel</span>';
    $html .= '<input type="text" name="cb_crypto"><br />';
    $html .= '<span>Date de validité</span>';
    $html .= '<select name="cb_valid_year">';
    for ($i=date('Y'); $i<(date('Y')+20); $i++) {
      $html .= '<option value="'.$i.'">'.$i.'</option>';
    }
    $html .= '</select>';
    $html .= '<select name="cb_valid_month">';
    for ($i=1; $i<13; $i++) {
      $html .= '<option value="'.$i.'">'.$i.'</option>';
    }
    $html .= '</select>';
    $html .= '</fieldset>';
    $html .= '<hr />';
    $html .= '<div><input type="radio" name="mode" value="paypal" /> Paiement avec Paypal</div>';
    $html .= '<input type="hidden" name="transaction_id" value="'.$payment->id.'">';
    $html .= '<input type="submit" value="Ok" />';
    $html .= '</form>';

    return $html;
  }

  public static function callback() {
    $req = S('req');
    $config = self::config();
    $params = $config['params'];

    $payment = Payment::getInstance((int) $req->transaction_id);
    $amount = str_replace('.', '', str_replace(',', '', $payment->amount)); //100,50 => 10050
    $method = $payment->getMethod();
    $order = $payment->getOrder();
    $user = $order->getUser();

    if ($order->isPaid()) {
      throw new Exception('order ('.$order->getId().') already paid [status='.$order->status.']');
    }
    if ($payment->isReceived()) {
      throw new Exception('payment ('.$payment->getId().') already received [order='.$order->getId().']');
    }

    $card = array('number' => $req->cb,
                  'validity' => $req->cb_valid_year.$req->cb_valid_month,
                  'crypto_key' => $req->cb_crypto,
                  'type'  => '');
    switch ($method->name) {
      case 'amex':
        $card['type'] = 'AMEX';
        break;
      case 'carte bleue':
        $card['type'] = 'CB';
        break;
      case 'carte bleue 2':
        $card['type'] = 'CB';
        break;
      case 'carte mastercard':
        $card['type'] = 'MASTERCARD';
        break;
      case 'carte visa':
        $card['type'] = 'VISA';
        break;
      default:
        throw new Exception('Unsuported card');
    }

    $doc = new DOMDocument();
    $service = $doc->createElement('service');
    $service->setAttribute('component', 'office');
    $service->setAttribute('name', 'author');

    $author = $doc->createElement('author');
    $author->setAttribute('merchant_id', $params['merchant_id']);
    $author->setAttribute('merchant_country', $params['merchant_country']);
    $author->setAttribute('transaction_id', $payment->id);
    $author->setAttribute('amount', $amount);
    $author->setAttribute('currency_code', '978'); //code monnaie (numerique)
    $author->setAttribute('card_number', $card['number']);
    $author->setAttribute('card_validity', $card['validity']);
    $author->setAttribute('card_type', $card['type']);
    $author->setAttribute('order_id', $order->id);

    /* optionnel */
    if (!empty($card['crypto_key'])) {
      $author->setAttribute('cvv_flag', '1');
      $author->setAttribute('cvv_key', $card['crypto_key']);
    }

    $service->appendChild($author);
    $doc->appendChild($service);
    $sreq = str_replace('<?xml version="1.0"?>'.PHP_EOL, '', $doc->saveXML());
    $socket = self::getServiceSocket($errno, $errstr);
    $doc = self::loadXml($socket, $sreq);

    if ($doc->getElementsByTagName('Error')->length == 0) {
      $author_resp = $doc->getElementsByTagName('author')->item(0);
      $state = ($author_resp->getAttribute('response_code') == '00') ? 'ok' : 'error';
      $dom = $author_resp;

      if ($state == 'ok') {
        $status = Payment::STATUS_RECEIVED;
        $description = 'success';
      }
      else {
        $status = Payment::STATUS_ERROR;
        $description = 'payment refused (code: '.$author_resp->getAttribute('response_code').')';
      }
    }
    else {
      $state = 'error';
      $dom = $doc->getElementsByTagName('Error')->item(0);

      $status = Payment::STATUS_ERROR;
      $description = 'failed';
    }

    $attrs = array('status'      => $status,
                   'description' => $description,
                   'datas'       => $doc->saveXml());
    $payment->commit($attrs);
    $payment->sendLetter();

    if ($payment->isLast()) {
      $order->setStatus(Order::STATUS_PAID);
    }
    else {
      $order->setStatus(Order::STATUS_ERROR);
    }

    $config['payment.response'] = $config['payment.response'] ?: '/payment.response.sipsofficeserver';
    $config['payment.cancel'] = $config['payment.cancel'] ?: '/payment.cancel';

    S('res')->sendRedirect($config['payment.response'].'/-/item/'.$order->getKey());
    // S('log')->debug(self::xmlParamsToHash($dom));
  }

  public static function callbackPaypal() {
    $req = S('req');
    $config = self::config();
    $params = $config['params'];

    $payment = Payment::getInstance((int) $req->transaction_id);
    $amount = str_replace('.', '', str_replace(',', '', $payment->amount)); //100,50 => 10050
    $method = $payment->getMethod();
    $order = $payment->getOrder();

    $doc = new DOMDocument();
    $service = $doc->createElement('service');
    $service->setAttribute('component', 'paypal');
    $service->setAttribute('name', 'setcontext');

    $context = $doc->createElement('setcontext');
    $context->setAttribute('merchant_id', $params['merchant_id']);
    $context->setAttribute('merchant_country', $params['merchant_country']);
    $context->setAttribute('transaction_id', $payment->id);
    $context->setAttribute('amount', $amount);
    $context->setAttribute('currency_code', '978'); //code monnaie (numerique)
    $context->setAttribute('pp_invoice_number', $order->id);

    $context->setAttribute('pp_return_url', 'http://'.P('app.domain').$config['payment.response'].'/-/item/'.$order->getKey());
    $context->setAttribute('pp_cancel_url', 'http://'.P('app.domain').$config['payment.cancel'].'/-/item/'.$order->getKey());

    /**/

    $service->appendChild($context);
    $doc->appendChild($service);
    $sreq = str_replace('<?xml version="1.0"?>'.PHP_EOL, '', $doc->saveXML());
    $socket = self::getServiceSocket($errno, $errstr);

    $doc = self::loadXml($socket, $sreq);

    if ($doc->getElementsByTagName('Error')->length == 0) {
      $context_resp = $doc->getElementsByTagName('setcontext')->item(0);
      $state = ($context_resp->getAttribute('response_code') == '00') ? 'ok' : 'error';

      if ($state == 'ok') {
        $token = $context_resp->getAttribute('pp_token');
        $paypal_url = $context_resp->getAttribute('pp_paypal_url');
        $payment->setAttribute('token', $token);
        S('res')->sendRedirect($paypal_url);
      }
      else {
        throw new Exception('invalid order');
      }
    }
    else {
      throw new Exception('internal error');
    }
  }

  public static function responsePaypal() {
    $req = S('req');
    $config = self::config();
    $params = $config['params'];

    $order = Order::getInstanceByKey($req->item);
    $payment = $order->getPayment();
    $amount = str_replace('.', '', str_replace(',', '', $payment->amount)); //100,50 => 10050
    $method = $payment->getMethod();

    // if ($req->token != $payment->token) {
      // throw new Exception('invalid token');
    // }

    /**/
    $doc = new DOMDocument();
    $service = $doc->createElement('service');
    $service->setAttribute('component', 'paypal');
    $service->setAttribute('name', 'getcontext');

    $context = $doc->createElement('getcontext');
    $context->setAttribute('merchant_id', $params['merchant_id']);
    $context->setAttribute('merchant_country', $params['merchant_country']);
    $context->setAttribute('transaction_id', $payment->id);
    $context->setAttribute('currency_code', '978'); //code monnaie (numerique)
    $context->setAttribute('amount', $amount);
    $context->setAttribute('pp_token', $payment->token);

    /**/

    $service->appendChild($context);
    $doc->appendChild($service);
    $sreq = str_replace('<?xml version="1.0"?>'.PHP_EOL, '', $doc->saveXML());
    $socket = self::getServiceSocket($errno, $errstr);

    $doc = self::loadXml($socket, $sreq);

    if ($doc->getElementsByTagName('Error')->length == 0) {
      $context_resp = $doc->getElementsByTagName('getcontext')->item(0);
      $state = ($context_resp->getAttribute('response_code') == '00') ? 'ok' : 'error';

      if ($state == 'ok') {
        $status = Payment::STATUS_RECEIVED;
        $description = 'success';
      }
      else {
        $status = Payment::STATUS_ERROR;
        $description = 'payment refused (code: '.$context_resp->getAttribute('response_code').')';
      }
    }
    else {
      $state = 'error';
      $status = Payment::STATUS_ERROR;
      $description = 'failed';
    }

    $attrs = array('status'      => $status,
                   'description' => $description,
                   'datas'       => $doc->saveXml());
    $payment->commit($attrs);
    $payment->sendLetter();

    if ($payment->isLast()) {
      $order->setStatus(Order::STATUS_PAID);
    }
    else {
      $order->setStatus(Order::STATUS_ERROR);
    }
  }

  public static function response() {}

  public static function checkOfficeComponent() {
    $config = self::config();
    $params = $config['params'];
    $socket = self::getServiceSocket($errno, $errstr);
    $sreq = '<service component="office" name="checkoffice"><checkoffice merchant_id="'.$params['merchant_id'].'" merchant_country="'.$params['merchant_country'].'"/></service>'.PHP_EOL;

    $doc = self::loadXml($socket, $sreq);

    if ($doc->getElementsByTagName('response')->length > 0) {
      $r = $doc->getElementsByTagName('response')->item(0);
      return array('state' => 'ok', 'message' => self::xmlParamsToHash($r));
    }
    else {
      S('log')->trace($doc->getElementsByTagName('response'), __METHOD__);
      return array('state' => 'error', 'message' => $r->getAttribute('message'));
    }
  }

  public static function checkPaypalComponent() {
    $config = self::config();
    $params = $config['params'];
    $socket = self::getServiceSocket($errno, $errstr);
    $sreq = '<service component="paypal" name="checkpaypal"><checkpaypal merchant_id="'.$params['merchant_id'].'" merchant_country="'.$params['merchant_country'].'"/></service>'.PHP_EOL;

    $doc = self::loadXml($socket, $sreq);

    if ($doc->getElementsByTagName('response')->length > 0) {
      $r = $doc->getElementsByTagName('response')->item(0);
      return array('state' => 'ok', 'message' => self::xmlParamsToHash($r));
    }
    else {
      S('log')->trace($doc->getElementsByTagName('response'), __METHOD__);
      return array('state' => 'error', 'message' => $r->getAttribute('message'));
    }
  }

  public static function checkPayidComponent() {
    $config = self::config();
    $params = $config['params'];
    $socket = self::getServiceSocket($errno, $errstr);
    $sreq = '<service component="payid" name="checkpayid"><checkpayid merchant_id="'.$params['merchant_id'].'" merchant_country="'.$params['merchant_country'].'"/></service>'.PHP_EOL;

    $doc = self::loadXml($socket, $sreq);

    if ($doc->getElementsByTagName('response')->length > 0) {
      $r = $doc->getElementsByTagName('response')->item(0);
      return array('state' => 'ok', 'message' => self::xmlParamsToHash($r));
    }
    else {
      S('log')->trace($doc->getElementsByTagName('response'), __METHOD__);
      return array('state' => 'error', 'message' => $r->getAttribute('message'));
    }
  }

  public static function sendReq($msg=null) {
    $msg = $msg ?: '<service component="office" name="author"><author merchant_id="221038718000000" merchant_country="en" transaction_id="000390" amount="1000" currency_code="978" card_number="1111222233334400" card_validity="201312" card_type="CB" order_id="816" cvv_flag="1" cvv_key="400"/></service>';
    $config = self::config();
    $params = $config['params'];
    if (strpos($req_msg, 'command') > 0) {
      $socket = self::getCommandSocket($errno, $errstr);       
    }
    else {
      $socket = self::getServiceSocket($errno, $errstr);      
    } 
    $sreq = $req_msg.PHP_EOL;
    
    $doc = self::loadXml($socket, $sreq);
    
    if ($doc->getElementsByTagName('response')->length > 0) {
      $r = $doc->getElementsByTagName('response')->item(0);
      return array('state' => 'ok', 'message' => self::xmlParamsToHash($r));
    }
    else {
      S('log')->trace($doc->getElementsByTagName('response'), __METHOD__);
      return array('state' => 'error', 'message' => $r->getAttribute('message'));
    }
  }

  public static function validateTransaction($order) {
    $config = self::config();
    $params = $config['params'];
    $socket = self::getServiceSocket($errno, $errstr);
    $payment = $order->getPayment();

    $sreq = '<service component="office" name="validate">'
            .  '<validate origin="Batch"'
            .           ' merchant_id="'.$params['merchant_id'].'"'
            .           ' merchant_country="'.$params['merchant_country'].'"'
            .           ' transaction_id="'.$payment->getId().'"'
            .           ' payment_date="'.date('Ymd', strtotime($order->paid_at)).'"'
            .           ' amount="'.($order->total_amount * 100).'"'
            .           ' currency_code="'.P('app.currency').'" />'
            .'</service>'.PHP_EOL;

    $doc = self::loadXml($socket, $sreq);

    if ($doc->getElementsByTagName('response')->length > 0) {
      $r = $doc->getElementsByTagName('response')->item(0);
      return array('state' => 'ok', 'message' => self::xmlParamsToHash($r));
    }
    else {
      S('log')->trace($doc->getElementsByTagName('response'), __METHOD__);
      return array('state' => 'error', 'message' => $r->getAttribute('message'));
    }
  }

  public static function cancelTransaction($order) {
    $config = self::config();
    $params = $config['params'];
    $socket = self::getServiceSocket($errno, $errstr);
    $payment = $order->getPayment();

    $sreq = '<service component="office" name="cancel">'
            .  '<cancel origin="Batch"'
            .         ' merchant_id="'.$params['merchant_id'].'"'
            .         ' merchant_country="'.$params['merchant_country'].'"'
            .         ' transaction_id="'.$payment->getId().'"'
            .         ' payment_date="'.date('Ymd', strtotime($order->paid_at)).'"'
            .         ' amount="'.($order->total_amount * 100).'"'
            .         ' currency_code="'.P('app.currency').'" />'
            .'</service>'.PHP_EOL;

    $doc = self::loadXml($socket, $sreq);

    if ($doc->getElementsByTagName('response')->length > 0) {
      $r = $doc->getElementsByTagName('response')->item(0);
      return array('state' => 'ok', 'message' => self::xmlParamsToHash($r));
    }
    else {
      S('log')->trace($doc->getElementsByTagName('response'), __METHOD__);
      return array('state' => 'error', 'message' => $r->getAttribute('message'));
    }
  }

}