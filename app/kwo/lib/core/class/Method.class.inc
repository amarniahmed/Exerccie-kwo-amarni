<?php

/***
PSP = Payment Service Provider
***/

class Method extends ActiveRecord {

  const TYPE_CREDITCARD = 3;
  const TYPE_CREDITACCOUNT = 4;
  const TYPE_CHECK = 7;
  const TYPE_GIFTCARD = 31;
  const TYPE_VOID = 41;

  const TYPE_MOBILE_SMS = 101;
  const TYPE_PHONE_IVR = 105;

  const FLAG_INSTALLEMENT = 1;

  public static $flags = array(self::FLAG_INSTALLEMENT => 'multiple');

  public static $types = array(1 => 'amex',
                               2 => 'credittransfer',
                               self::TYPE_CREDITCARD => 'creditcard',
                               self::TYPE_CREDITACCOUNT => 'creditaccount',
                               5 => 'digitalwallet',
                               6 => 'prepaidcard',
                               self::TYPE_CHECK => 'check',
                               8 => 'voucher',
                               self::TYPE_GIFTCARD => 'giftcard',
                               self::TYPE_VOID => 'void',
                               self::TYPE_MOBILE_SMS => 'mobile-sms',
                               102 => 'mobile-internet+',
                               self::TYPE_PHONE_IVR => 'phone-ivr', // audiotel
                               110 => 'phone-isp');

  public function request($order, $amount=null) {
    $amount = $amount ?: $order->total_amount;
    if ($order->isPaid()) {
      S('log')->error('order ('.$order->getId().') already paid '.
                      '[status='.$order->status.']',
                      __METHOD__);
      throw new Exception('order ('.$order->getId().') already paid');
    }
    $args = array('order_id' => $order->getId(),
                  'method_id' => $this->getId(),
                  'status' => Payment::STATUS_PENDING);
    $id = (int) Payment::id($args);
    $id = $id > 0 ? $id : true;
    $payment = Payment::getInstance($id);
    $payment->commit(array('order_id' => $order->getId(),
                           'method_id' => $this->getId(),
                           'amount' => $amount));
    $payment->setVisit(S('req'));
    $class = self::loadClass($this->getPsp()->getName());
    /*
    Ajout Sylvain pour FIANET
    $order->setAttribute('user_ip', S('req')->getClient('address'));
    */
    $html = call_user_func(array($class, 'request'), $payment);
    $payment->sendLetter();
    return $html;
  }

  public function asFinderHash($h) {
    $h['type'] = self::$types[$this->type];
    return $h;
  }

  public static function loadClass($name) {
    $name = (string) $name;
    $class = ucfirst($name);
    require LIB_PATH.'/core/class/psp/'.$class.'.class.inc';
    return $class;
  }

  public static function callback() {
    $req = S('req');
    if ($req->hasAttribute('psp')) {
      $class = $req->getAttribute('psp');
    }
    elseif ($req->hasAttribute('ips')) {
      S('log')->trace('ips param is deprecated', __METHOD__);
      $class = $req->getAttribute('ips');
    }
    else {
      throw new Exception('unknown psp');
    }
    $class = self::loadClass($class);
    call_user_func(array($class, 'callback'));
  }

  public function getType() {
    return self::$types[$this->type];
  }

  public static function label($id) {
    $hash = self::names();
    return $hash[$id];
  }

  public static $names = null;

  public static function names() {
    if (!is_array(self::$names)) {
      self::$names = S('dbo')->asHash('SELECT id, name'
                                      .' FROM '.T('payment')
                                      .' WHERE status=1');
    }
    return self::$names;
  }

}