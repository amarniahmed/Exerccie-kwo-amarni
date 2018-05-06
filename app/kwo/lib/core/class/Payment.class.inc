<?php

class Payment extends ActiveRecord {

  const FLAG_EMAIL_SENT = 1;

  public static $flags = array(self::FLAG_EMAIL_SENT => 'email envoyé');

  const STATUS_RECEIVED = 10;
  const STATUS_PENDING = 0;
  const STATUS_CANCELED = -50;
  const STATUS_ERROR = -100;

  public static $statuses = array(self::STATUS_RECEIVED => 'reçu',
                                  self::STATUS_PENDING => 'en attente',
                                  self::STATUS_CANCELED => 'annulé',
                                  self::STATUS_ERROR => 'erreur');

  public function sendLetter() {
    try {
      $method = $this->getMethod();
      $code = null;
      if ($this->status == Payment::STATUS_RECEIVED) {
        $code = $method->letter_received;
      }
      elseif ($this->status == Payment::STATUS_PENDING) {
        $code = $method->letter_pending;
      }
      if (strlen($code) < 2) return ;
      $order = $this->getOrder();      
      $user = $order->getUser();
      $bindings = $order->getAttributes();
      $bindings['details'] = H::get('/order.details',
                                    array('id' => $order->id));
      $bindings['payment_amount'] = H::amount($this->amount);
      Letter::send($code, $user, $bindings);
    }
    catch (Exception $e) { }
  }

  public function onBeforeCommit(&$attrs) {
    if (is_array($attrs['datas'])) {
      $attrs['datas'] = serialize($attrs['datas']);
    }
  }

  public function isReceived() {
    return $this->status == self::STATUS_RECEIVED;
  }

  public function asEditHash() {
    $h = array('datas' => array());
    if (strlen($this->datas) > 3) {
      $h['datas'] = unserialize($this->datas);
    } 
    return $h;
  }

  public function isLast() {
    if ($this->status != self::STATUS_RECEIVED) return false;
    $order = Order::getInstance((int) $this->order_id);
    $s = S('dbo')->fetchValue('SELECT SUM(amount)'
                             .' FROM '.T('payment')
                             .' WHERE status='.self::STATUS_RECEIVED
                             .  ' AND order_id='.$order->getId());
    return ceil($s) >= ceil($order->total_amount);
  }

  public function getName($locale=null) {
    $method = $this->getMethod();
    $name = $method->getName();
    $psp = $method->getPsp();
    $name .= ' / '.$psp->getName();
    return $name;
  }

  public function asListHash() {
    $opts = array('callback' => 'reload');
    $h = array('type' => Item::edit($this, $opts),
               'statut' => self::$statuses[$this->status],
               'montant' => $this->amount,
               'création' => $this->placed_at);
    if (!DH::isNull($this->paid_at)) {
      $h['paiement'] = $this->paid_at;
    }
    if (!empty($this->code)) {
      $h['uid'] = $this->code;
    }
    if (!empty($this->description)) {
      $h['infos'] = $this->description;
    }
    return $h;
  }
  
}
