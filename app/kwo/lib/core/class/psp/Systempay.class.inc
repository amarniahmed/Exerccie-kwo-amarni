<?php

/**

https://systempay.cyberpluspaiement.com/vads-merchant/

https://systempay.cyberpluspaiement.com/html/contributions.html
2.2_Guide_d_implementation_formulaire_Paiement_V2.pdf

parameters : (dans l'interface d'admin)
$parameter['shop.mode'] = 'test' || 'production';
$parameter['shop.params'] = array('site_id' => '123',
                                                    'certificate' => '456');

CB OK : 4970 1000 0000 0003

PV recette : cyberplus.paiement@paiements.natixis.fr

Support : 0811 363 364 - cyberplus.paiement@paiements.natixis.fr

Parametrage SERVEUR-A-SERVEUR
Paramétrage / boutique / Configuration : URL serveur
http://domain/payment.callback/-/psp/systempay

**/

class Systempay
{
  public static $languages = array('de', 'en', 'zh', 'es', 'fr', 'it', 'jp');

  private static function config() {
    $config = P('shop.psp');
    $config = $config['systempay'];
    return $config;
  }

  public static function request($order) {
    $config = self::config();
    $mode = $config['mode'] == 'test' ? 'test' : 'production';
    $payment = $order->getPayment();
    $options = SH::asHash($payment->options);
    $params = $config['params'];
    $user = $order->getUser();
    $customer = $order->getCustomer();
    $amount = (int) (number_format($order->total_amount, 2, '.', '') * 10 * 10);
    $args = array('item' => $order->getKey());

    $url = 'https://systempay.cyberpluspaiement.com/vads-payment/';

    $language = I18N::locale2code($user->locale);
    $language = in_array($language, self::$languages) ? $language : 'fr';

    $attrs = array();
    $attrs['vads_action_mode'] = 'INTERACTIVE';
    $attrs['vads_amount'] = $amount;
    $attrs['vads_capture_delay'] = '';
    $attrs['vads_currency'] = 978; // ISO-4217
    $attrs['vads_cust_country'] = strtoupper(Country::code($user->country_id));
    $attrs['vads_cust_email'] = $user->email;
    $attrs['vads_cust_id'] = $user->getId();
    $attrs['vads_ctx_mode'] = strtoupper($mode);
    $attrs['vads_language'] = $language;
    $attrs['vads_order_id'] = P('app.name').'-'.$order->getId();
    $attrs['vads_page_action'] = 'PAYMENT';
    $attrs['vads_payment_cards'] = '';
    $attrs['vads_payment_config'] = 'SINGLE';
    $attrs['vads_return_mode'] = 'POST';
    $attrs['vads_site_id'] = $params['site_id'];
    $attrs['vads_trans_date'] = date('YmdHis', time() - date('Z'));
    $attrs['vads_trans_id'] = ($order->getId() + time()) % 899999;
    $attrs['vads_validation_mode'] = '';
    $attrs['vads_version'] = 'V2';
    $attrs['vads_url_cancel'] = H::url('/payment.cancel', $args);
    $attrs['vads_url_error'] = H::url('/payment.cancel', $args);
    $attrs['vads_url_referral'] = H::url('/payment.cancel', $args);;
    $attrs['vads_url_refused'] = H::url('/payment.cancel', $args);
    $attrs['vads_url_success'] = H::url('/payment.response', $args);

    // TODO : mettre trans_id dans le code du Payment

    ksort($attrs);
   
    $data = '';
    foreach ($attrs as $key => $value) {
      if (substr($key, 0, 5) != 'vads_') continue ;
      $data .= $value.'+';
    }
    $data .= $params['certificate'];

    $attrs['signature'] = sha1($data, false);

    $html = array();
    $html[] = '<form method="post" action="'.$url.'">';
    foreach ($attrs as $key => $value) {
      $html[] = '<input type="hidden" name="'.$key.'" value="'.$value.'" />';
    }
    $html[] = '</form>';
    $html[] = H::trigger('$(this).previous().submit()');
    $html = join("\n", $html);
    return $html;

  }

  public static function callback() {
    $req = S('req');
    $config = self::config();
    $params = $config['params'];

    $attrs = $req->getAttributes();
    ksort($attrs);
    
    $data = '';
    foreach ($attrs as $key => $value) {
      if (substr($key, 0, 5) != 'vads_') continue ;
      $data .= $value.'+';
    }
    $data .= $params['certificate'];
    
    $signature = sha1($data, false);
    if ($attrs['signature'] != $signature) {
      S('log')->error('bad signature', __METHOD__);
      throw new Exception('invalid signature');
    }

    list ($app, $order_id) = explode('-', $attrs['vads_order_id']);
    $order = Order::getInstance((int) $order_id);
    if ($order->isPaid()) {
      throw new Exception('order ('.$order->getId().') already paid [status='.$order->status.']');
    }
    $result = (int) $attrs['vads_result'];
    if ($result === 0) {
      $status = Order::STATUS_PAID;
      $order->commit(array('status' => $status,
                           'payment_transaction_id' => $attrs['vads_auth_number'],
                           'payment_comment' => '',
                           'payment_params' => serialize($attrs)));
    }
    else {
      $cmt = $result.': '.self::$errors[$result].' ('.$attrs['vads_extra_result'].')';
      $order->commit(array('payment_comment' => $cmt,
                           'payment_params' => serialize($attrs)));
    }

  }

  public static function response() {}

  public static $errors = array(0 => 'Paiement réalisé avec succès',
                                2 => 'Le commerçant doit contacter la banque du porteur',
                                5 => 'Paiement refusé',
                                17 => 'Annulation client',
                                30 => 'Erreur de format de la requête. A mettre en rapport avec la valorisation du champ extra_result',
                                96 => 'Erreur technique lors du paiement');

}