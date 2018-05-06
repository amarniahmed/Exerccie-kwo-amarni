<?php

/**

$parameter['shop.psp'] = array('allopass' => array('mode' => 'test',
                                                   'payment.response' => '',
                                                   'payment.cancel' => '',
                                                   'params' => array('site_id' => '298548')));
/**/

class Allopass 
{
  const API_URL = 'https://payment.allopass.com/api/onetime';
  const TYPE_AUDIOTEL = 'premium-calling';
  const TYPE_SMS = 'premium-sms';
  
  private static function config() {
    $config = (array) P('shop.psp');
    $config = (array) $config['allopass'];
    return $config;
  }
  
  public static function request($payment) {
    $amount = $payment->amount;
    $method = $payment->getMethod();
    $order = $payment->getOrder();
    
    $config = self::config();
    $params = $config['params'];
    $user = $order->getUser();
    
    if (isset($config['mode']) && $config['mode'] == 'test') {
      $product_id = '1273491';
    }
    else {
      $purchase = $order->getPurchases()->first();
      /** pour jv1t **/
      if ($purchase->model_id == I('option'))
        $purchase = Option::getInstance($purchase->record_id);
      /***/
      $product_id = $purchase->allopass_id;
    }
    
    $error = '';
    if ($product_id == '') {
      S('log')->error('Numéro de produit inexistant', __METHOD__);
      $error = '<span class="error">Numéro de produit inexistant</span>';
    }
    
    $html = self::htmlPricing($method).'
    <!--a href="https://payment.allopass.com/buy/buy.apu?ids='.$params['site_id'].'&idd='.$product_id.'" style="border:0" target="_blank">
      <img src="https://payment.allopass.com/static/buy/button/fr/162x56.png" style="border:0" alt="Buy now!" />
    </a-->
    <form id="form_allopass" action="/payment.request.allopass" method="post">
      <input type="hidden" value="'.$params['site_id'].'" name="site_id">
      <input type="hidden" value="'.$product_id.'" name="product_id">'.$error.'
      <input type="text" name="code[]" value="" '.($error == '' ? '' : 'disabled="disabled"').'>
      <input type="hidden" name="merchant_transaction_id" value="'.$payment->id.'">
      <input type="submit" value="Ok" '.($error == '' ? '' : 'disabled="disabled"').' />
    </form>';

    return $html;
  }
  
  public static function callback() {
    $req = S('req');
    $xml = $req->xml;
    $payment_id = $req->payment_id;
    
    $config = self::config();  
    $response_url = $config['payment.response'] ?: '/payment.response';
    $cancel_url = $config['payment.cancel'] ?: '/payment.cancel';


    $doc = new DOMDocument();
    $doc->loadXML($xml);
    
    $payment = Payment::getInstance((int) $payment_id);
    $order = $payment->getOrder();
    
    if ($order->isPaid()) {
      throw new Exception('order ('.$order->getId().') already paid [status='.$order->status.']');
    }
    if ($payment->isReceived()) {
      throw new Exception('payment ('.$payment->getId().') already received [order='.$order->getId().']');
    }

    $code_resp = $doc->getElementsByTagName('response')->item(0)->getAttribute('code');
    $node_status = $doc->getElementsByTagName('status')->item(0);
    $node_description = $doc->getElementsByTagName('status_description')->item(0);
    $code_status = $node_status->nodeValue;
    // S('log')->debug($code_resp);
    // S('log')->debug($code_status);
    // exit();
    if ($code_resp == 0 && $code_status == 0) {
      $status = Payment::STATUS_RECEIVED;
      $description = $node_description->nodeValue;
    }
    else {
      $status = Payment::STATUS_ERROR;
      $message = $doc->getElementsByTagName('response')->item(0)->getAttribute('message');
      $description = ($code_resp === 0) ? 'code '.sprintf('%02d', $code_resp).' - '.$message : 'status '.sprintf('%02d', $code_status).' - '.$node_description->nodeValue;
    }
    
    $attrs = array('status'      => $status,
                   'description' => $description,
                   'datas'       => $xml);
    $payment->commit($attrs);
    $payment->sendLetter();
    
    if ($payment->isLast()) {
      $order->setStatus(Order::STATUS_PAID);
    }
    // else {
      // $order->setStatus(Order::STATUS_PARTLY_PAID);
    // }

    // redirection 
    S('res')->sendRedirect($response_url.'/-/item/'.$order->getKey());
  }

  public static function response() {}
  
  
  public static function getPricing($method) {
    $config = self::config();
    $params = $config['params'];
    $payment_type = ($method->type == METHOD::TYPE_MOBILE_SMS) ? self::TYPE_SMS : self::TYPE_AUDIOTEL;
    
    
    $ip = S('req')->getClient('address');
    $url = self::API_URL.'/pricing.apu';
    $hreq = HttpRequest::getInstance($url);
    $hreq->setParams(array('site_id'             => $params['site_id'],
                           'customer_ip'         => $ip,
                           'only4ip'             => true,
                           'reference_currency'  => 'EUR',
                           'type'                => $payment_type));
    
    $hreq->setMethod('GET');
    $hresp = $hreq->send();
    
    $doc = new DOMDocument();
    $doc->loadXML($hresp->getContent());
    
    $markets = $doc->getElementsByTagName('market');
    $datas = array();
    foreach ($markets as $market) {
      $pricepoint = $market->getElementsByTagName('pricepoint')->item(0);
      $price = $pricepoint->getElementsByTagName('price')->item(0);
      
      $temp = array('country'  => $market->getAttribute('country'),
                    'type'     => $pricepoint->getAttribute('type'),
                    'amount'   => $price->getAttribute('amount'),
                    'currency' => $price->getAttribute('currency'));
      
      if ($payment_type == 'premium-sms') {
        $keyword = $pricepoint->getElementsByTagName('keyword')->item(0);
        $temp['code_sms'] = $keyword->getAttribute('name');
        $temp['num_sms'] = $keyword->getAttribute('shortcode');
      }
      else {
        $phone = $pricepoint->getElementsByTagName('phone_number')->item(0);
        $temp['phone'] = $phone->getAttribute('value');
      }
      $datas[] = $temp;
    }
    
    return $datas;
  }
  
  public static function htmlPricing($method) {
    $pricing = self::getPricing($method);
    $html = '';
    foreach ($pricing as $p) {
      if ($p['currency'] == 'EUR') {
        $html .= '<div class="devise-allopass">
          <span class="country">'.$p['country'].'</span>
          <span class="type">'.$p['type'].'</span>
          <div>
            <span class="amount">'.$p['amount'].'</span>
            <span class="currency">'.$p['currency'].'</span>
          </div>
          <div>';
        $html .= (empty($p['phone'])) ? 'Envoyez "'.$p['code_sms'].'" au "'.$p['num_sms'].'"' : 'Appelez le numéro surtaxé "'.$p['phone'].'"';
        $html .= '</div></div>';
      }
    }
    return $html;
  }
}












