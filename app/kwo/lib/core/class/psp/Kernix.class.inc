<?php

class Kernix {

  private static function config() {
    $config = P('shop.psp');
    $config = $config['check'] ? $config['check'] : array();
    return $config;
  }

  public static function request($payment) {
    $method = $payment->getMethod();
    $order = $payment->getOrder();

    $config = self::config();
    $response_url = $config['payment.response'] ?: '/payment.response';

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    if ($method->type == Method::TYPE_VOID) {
      if ($order->isPaid()) {
        throw new Exception('order ('.$order->getId().') already paid [status='.$order->status.']');
      }
      $order->setStatus(Order::STATUS_PAID);
    }
    elseif ($method->type == Method::TYPE_CREDITACCOUNT) {
      if ($order->isPaid()) {
        throw new Exception('order ('.$order->getId().') already paid [status='.$order->status.']');
      }
      $user = $order->getUser();
      $credit = $user->getCredit();
      if ($credit < $order->total_amount) {
        throw new Exception('insufficient credits for payment of order ('.$order->getId().') : '.
                            'credits('.$credit.') ; total_amount('.$order->total_amount.')');
      }
      $order->setStatus(Order::STATUS_PAID);
    }
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------

    $html  = '<form method="post" target="_top" action="'.$response_url.'" id="kernix-payment-form">'."\n";
    $html .= ' <input type="hidden" name="item" value="'.$order->getKey().'" />'."\n";
    $html .= '</form>'."\n";
    $html .= '<img src="'.PIX_URL.'/empty.gif" onload="$(\'kernix-payment-form\').submit()" />';

    return $html;
  }

  public static function callback() {}

  public static function response() {}

}

// BACKUP du 24/04/2013 12h05
/*
<?php

class Kernix {

  private static function config() {
    $config = P('shop.psp');
    $config = $config['check'] ? $config['check'] : array();
    return $config;
  }

  public static function request($payment) {

    $method = $payment->getMethod();
    $order = $payment->getOrder();

    $config = self::config();
    $response_url = $config['payment.response'] ?: '/payment.response';

    $html  = '<form method="post" target="_top" action="'.$response_url.'" id="kernix-payment-form">'."\n";
    $html .= ' <input type="hidden" name="item" value="'.$order->getKey().'" />'."\n";
    $html .= '</form>'."\n";
    $html .= '<img src="'.PIX_URL.'/empty.gif" onload="$(\'kernix-payment-form\').submit()" />';

    return $html;
  }

  public static function callback() {}

  public static function response() {}

}
*/