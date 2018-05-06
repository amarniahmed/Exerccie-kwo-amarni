<?php

try {

  Psp::load('allopass');

  $url = Allopass::API_URL.'/validate-codes.apu';
  $hreq = HttpRequest::getInstance($url);
  $hreq->setMethod('POST');
  $hreq->setParams($req->getAttributes());
  $hresp = $hreq->send();
  
  // S('log')->debug($hresp->getContent());
  
  $url = '/payment.callback/-/psp/allopass';
  $hreq = HttpRequest::getInstance($url);
  $hreq->setMethod('POST');
  $params = array('xml'        => $hresp->getContent(),
                  'payment_id' => $req->merchant_transaction_id);
  $hreq->setParams($params);
  $hreq->send();
  
}
catch (Exception $e) { $err->add($e); }
