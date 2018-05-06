<?php

class Giftcard extends ActiveRecord implements Purchasable {

  const FLAG_DIGITAL = 1;

  const STATUS_OFFLINE = 0;
  const STATUS_ONLINE = 10;

  public static $flags = array(
    self::FLAG_DIGITAL => 'gift-card numérique'
  );

  public static $statuses = array(
    self::STATUS_OFFLINE => 'hors ligne',
    self::STATUS_ONLINE => 'en ligne'
  );

  public function getAttribute($key) {
    if ($key == 'getKey') return $this->getKey();
    if ($key == 'option_name') return l('amount') . ' : ' . H::amount($this->getPrice());
    return parent::getAttribute($key);
  }

  protected function onBeforeCommit(&$attrs) {
    $attrs['locales'] = P('app.locales');
  }

  public function asPurchaseListHash($purchase) {
    $ht = array();
    if (Discount::exists($filters = array('name' => 'purchase-' . $purchase->getId()))) {
      $discount = Discount::getInstance($filters);
      $ht['coupon'] = Item::edit($discount, array('view' => 'item'));
      $attrs = array('href' => '' . '/doc/madlords/pdf/' . $discount->code . '.pdf',
                     'target' => '_blank');

      $ht['PDF'] = H::elt('a', 'Télécharger', $attrs);
    }

    return $ht;
  }

  public function getDiscountCode($order, $purchase, $qty) {
    $user = $order->getUser();
    return 'gc-' . $this->getId() . '-' . $user->getId() . '-' . $purchase->getId() . '-' . $qty;
  }

  /* ----- INTERFACE PURCHASABLE ----- */
  public function asPurchaseHash($order, $opts) {
    // code name description price tax image
    $hash = array();
    $hash['code'] = '';
    $hash['name'] = $this->name;
    $hash['description'] = '';
    $hash['image'] = '/' . '';
    $hash['tax'] = '';
    $hash['price'] = $this->getPrice();
    return $hash;
  }

  public function getAvailability($opts) {
    return $this->isDigital() ? 10 : $this->stock;
  }

  public function getPrice() {
    return $this->amount;
  }

  public function getThreshold($opts) {
    return 10;
  }

  public function isDigital() {
    return $this->hasFlag(self::FLAG_DIGITAL);
  }

  public function isPurchasable() {
    return $this->isDigital() || $this->stock > 0;
  }

  public function onOrderCancel($order, $purchase) {
    $user = $order->getUser();
    for ($qty = 1; $qty <= $purchase->quantity; $qty++) {
      $code = $this->getDiscountCode($order, $purchase, $qty);
      // Annulation du coupon lié
      if (!Discount::exists($code)) {
        return;
      }
      $discount = Discount::getInstance($code);
      $discount->destroy();
    }
  }

  public function onOrderPaid($order, $purchase) {
    $user = $order->getUser();
    for ($qty = 1; $qty <= $purchase->quantity; $qty++) {
      $code = $this->getDiscountCode($order, $purchase, $qty);
      // génération d'un coupon basé sur le montant
      $discount = Discount::getInstance();
      $discount->commit(array('model_id' => Model::id('purchase'),
                              'record_id' => $purchase->getId(),
                              'app' => P('app.name'),
//                            'available_from' => DH::today(),
//                            'available_to' => DH::today(),
                              'code' => $code,
                              'name' => 'purchase-' . $purchase->getId(),
                              'status' => Discount::STATUS_ONLINE,
                              'type' => Discount::TYPE_CART,
                              'flags' => array(Discount::FLAG_SINGLE_USE),
                              'locales' => P('app.locales')
      ));
      foreach (P('app.locales') as $locale) {
        $discount->setProperty('message', $this->getProperty('description', $locale), $locale);
      }

      // ajout des conditions
      $row = array('model_id' => Model::id('order'),
                   'attribute' => 'coupon',
                   'discount_id' => $discount->getId(),
                   'value' => $code);
      $this->dbo->insert(Discount::TABLE_CONDITION, $row);
      /*$row = array('model_id' => Model::id('order'),
                  'attribute' => 'user',
                  'discount_id' => $discount->getId(),
                  'value' => $user->getId());
     $this->dbo->insert(Discount::TABLE_CONDITION, $row);*/

      // ajout des actions
      $row = array('rate' => (float) $this->amount,
                   'model_id' => Model::id('order'),
                   'attribute' => 'amount',
                   'discount_id' => $discount->getId());
      $this->dbo->insert(Discount::TABLE_ACTION, $row);
    }
  }

  public function onPurchase($purchase) {
    // TODO: Implement onPurchase() method.
  }

}