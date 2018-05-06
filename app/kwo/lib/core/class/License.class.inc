<?php

class License extends ActiveRecord implements Purchasable {

  const ALERT_DAY = 1;
  const ALERT_WEEK = 2;
  const ALERT_MONTH = 4;

  public static $alerts = array(self::ALERT_DAY => 'la veille',
                                self::ALERT_WEEK => 'une semaine avant',
                                self::ALERT_MONTH => 'un mois avant');

  public function asPurchaseHash($order, $opts) {
    return array('code' => $this->code,
                 'name' => $this->getProperty('title'),
                 'price' => $this->price,
                 'tax' => $this->tax);
  }

  public function onOrderPaid($order, $purchase) {
    $user = $order->getUser(); 
    $user->addPrivilege($this->privilege_id, 
                        array('duration' => $this->duration));
  }

  public function onOrderCancel($order, $purchase) { 
    $this->dbo->query('DELETE FROM '.User::TABLE_PRIVILEGE
                      .    ' WHERE privilege_id='.$this->privilege_id
                      .      ' AND user_id='.$order->getUser()->getId());
  }

  public function onPurchase($purchase) { }

  public function isDigital() { return true; }
  public function hasDiscount() { return false; }

  public function isPurchasable() { return true; }

  public function getPrice() {
    return $this->price * ($this->tax + 100) / 100;
  }

   public function getDisplayPrice() {
     return $this->price * ($this->tax + 100) / 100;
   }

  public function getThreshold($opts) {
    return 1;
  }

  public function getAvailability($opts) {
    return 100;
  }

}