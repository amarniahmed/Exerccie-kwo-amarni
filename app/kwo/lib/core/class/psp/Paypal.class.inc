<?php 

class Paypal {
  
  public static function request($order) {

    $server_url = 'https://www.paypal.com/cgi-bin/webscr';
    if (P('shop.mode') == 'test')
      $server_url = 'https://www.sandbox.paypal.com/cgi-bin/webscr';
    $parameters = P('shop.params');
    if (!is_array($parameters)) {
      $parameters['business'] = 'fxbois@kernix.com';
    }
    $url = H::url('/');

    $parameters += array('cmd' => '_xclick',
                         'item_name' => 'Commande #'.$order->getId(),
                         'item_number' => $order->getId(),
                         'amount' => number_format($order->total_amount, 2, '.', ''),
                         'currency_code' => 'EUR',
                         'no_note' => '1',
                         'notify_url' => $url.'payment.callback/-/psp/paypal?id='.$order->getId(),
                         'return' => $url.'payment.response?id='.$order->getId());
    
    $form = Template::render('psp.paypal', 
                             array('server_url' => $server_url, 
                                   'hash' => $parameters));
    return $form;
  }
  
  public static function callback($order) {
    //    "Completed", "Pending"
    $req = S('req');
    $regex = str_replace('.', '\.', join('|', self::$range));
    if (!preg_match('/^('.$regex.')/', $req->getClient('address'))) {
      throw new Exception('bad source IP ('.$order->getId().')');
    }
    if ($req->payment_status != 'Completed') {
      $req->debug();
      return ;
    }
    $order->commit(array('status' => Order::STATUS_PAID));
  }

  public static function response() {}

  private static $range = array('216.113.188', '64.4.241', '192.168', '173.0.81');

}

