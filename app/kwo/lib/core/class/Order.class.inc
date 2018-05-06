<?php

/*
http://code.google.com/apis/checkout/developer/index.html#order_processing_api
NEW - PROCESSING - DELIVERED
*/

class Order extends ActiveRecord
{
  const TABLE_DISCOUNT = 'core_discount_order';

  const TTL = 1200;

  /*
  // old VAT
  const SHIPPING_TAX = 19.6;
  const HANDLING_TAX = 19.6;
  */

  const SHIPPING_TAX = 20;
  const HANDLING_TAX = 20;

  const FLAG_DIGITAL = 1;
  const FLAG_EXPORTED = 2;
  const FLAG_DISCOUNT = 4;
  const FLAG_ITEM_RETURNED = 8;
  const FLAG_HANDLING = 16;

  public static $flags = array(self::FLAG_DIGITAL => 'digitale',
                               self::FLAG_EXPORTED => 'exportée',
                               self::FLAG_DISCOUNT => 'réduction',
                               self::FLAG_ITEM_RETURNED => 'retour demandé',
                               self::FLAG_HANDLING => 'handling');

  const STATUS_CANCELLED = -100;
  const STATUS_ERROR = -50;
  const STATUS_PENDING = 1;
  const STATUS_NONE = 0;
  const STATUS_PARTLY_PAID = 5;
  const STATUS_PAID = 10;
  const STATUS_PROCESSING = 20;
  const STATUS_PROCESSED = 100;
  //DELIVERED

  public static $statuses = array(self::STATUS_PROCESSED => 'payée, envoyée',
                                  self::STATUS_PROCESSING => 'payée, traitement',
                                  self::STATUS_PAID => 'payée',
                                  self::STATUS_PARTLY_PAID => 'payée patiellement',
                                  self::STATUS_PENDING => 'attente règlement',
                                  self::STATUS_NONE => 'en cours',
                                  self::STATUS_CANCELLED => '** Annulée **',
                                  self::STATUS_ERROR => '** Erreur **');

  const SHIPPING_METHOD_DELIVERY = 1;
  const SHIPPING_METHOD_PICKUP = 2;

  public function asFinderHash($h) {
    foreach ($h as $key => &$value) {
      if ($key === '_color') {
        if ($this->status < self::STATUS_PENDING) { $value = 'red'; }
        elseif ($this->status == self::STATUS_PENDING) { $value = 'orange'; }
        elseif ($this->status == self::STATUS_PROCESSING) { $value = 'blue'; }
        elseif ($this->status < self::STATUS_PROCESSED) { $value = 'yellow'; }
        else { $value = 'green'; }
      }
      elseif ($key === 'status') {
        $value = c(l(self::$statuses[$this->status]));
      }
    }
    return $h;
  }

  public function getShippingAmount() {
    return (float) $this->shipping_amount;
  }

  public function getPurchaseCount() {
    $quantity = 0;
    foreach ($this->getPurchases() as $purchase) {
      $quantity += $purchase->quantity;
    }
    return $quantity;
  }

  public function getPurchaseAmountWithTax() {
    $amount = 0;
    foreach ($this->getPurchases() as $purchase) {
      $amount += $purchase->quantity * $purchase->getPriceWithTax();
    }
    return $amount;
  }

  public function getPurchaseAmountWithoutTax() {
    $amount = 0;
    foreach ($this->getPurchases() as $purchase) {
      $amount += $purchase->quantity * $purchase->getPriceWithoutTax();
    }
    return $amount;
  }

  public function isDigital() {
    foreach ($this->getPurchases() as $purchase) {
      if (!$purchase->hasFlag(Purchase::FLAG_DIGITAL)) return false;
    }
    return true;
  }

  public function addPurchase($item, $opts=null) {
    $opts = (array) $opts;
    $opts['quantity'] = $opts['quantity'] ?: 1;

    if ($item instanceof Bundle) {
      $item->purchase($this);
      return $item;
    }
    if (!($item instanceof Purchasable)) {
      S('log')->error($item, __METHOD__);
      throw new Exception(l('error.purchase.forbidden'));
    }
    $variations = (string) $opts['variations'];
    $quantity = (int) $opts['quantity'];
    $pack_id = $opts['pack_id'] >= 1 ? (int) $opts['pack_id'] : 0;

    if ($this->isClosed()) {
      S('log')->error('cart already ordered ('.$this->getId().')',
                        __METHOD__);
      throw new Exception(l('error.cart.invalid'));
    }

    $availability = $item->getAvailability($opts);
    $threshold = $item->getThreshold($opts);

    $cart_quantity = (int) $this->dbo->fetchValue('SELECT SUM(quantity)'
                                                  .' FROM '.T('purchase')
                                                  .' WHERE order_id='.$this->getId()
                                                  .  ' AND model_id='.$item->getModelId()
                                                  .  ' AND record_id='.$item->getId()
                                                  .  ' AND variations=:variations:'
                                                  .' GROUP BY model_id, record_id',
                                                  array(':variations:' => $variations));

    $availability = $availability - $cart_quantity;

    $args = array('order_id' => $this->getId(),
                  'model_id' => $item->getModelId(),
                  'record_id' => $item->getId(),
                  'pack_id' => $pack_id,
                  'variations' => $variations);
    if (Item::exists('purchase', $args)) {
      $purchase = Purchase::getInstance($args);
      $availability += $purchase->quantity;
      if (!$item->isPurchasable() || $availability < 1) {
        $purchase->destroy();
        throw new Exception(l('error.product.stock'));
      }
      $quantity = $purchase->quantity + $quantity;
      $quantity = $quantity > $availability ? $availability : $quantity;
      if ($threshold > 0) {
        $quantity = $quantity > $threshold ? $threshold : $quantity;
      }
      $purchase->commit(array('quantity' => $quantity));
      $purchase->setMeta(Meta::FIELD_UPDATED_AT, DH::now());
      if (!DH::isNull($this->expire_at)) {
        $this->commit(array('expire_at' => DH::when(self::TTL)));
      }
      $purchases = array($purchase->getId() => $quantity);
      S('app')->hook('onCartUpdate',
                   array('order' => $this,
                         'purchases' => $purchases));
      return $purchase;
    }

    $quantity = $quantity > $availability ? $availability : $quantity;
    if ($threshold > 0) {
      $quantity = $quantity > $threshold ? $threshold : $quantity;
    }

    if (!$item->isPurchasable() || $availability < 1) {
      throw new Exception(l('error.product.stock'));
    }

    $attrs = array_merge($args,
                         array('quantity' => $quantity),
                         $item->asPurchaseHash($this, $opts));

    $purchase = Purchase::getInstance();
    $purchase->commit($attrs);
    if (method_exists($item, 'onAfterPurchase')) {
      $item->onAfterPurchase($purchase);
    }
    if ($item->isDigital()) {
      $purchase->setFlag(Purchase::FLAG_DIGITAL);
    }
    $purchase->setVisit(S('req'));
    $item->onPurchase($purchase);

    if (!empty($purchase->name)) {
      $this->setAttribute('name', $purchase->name);
    }

    return $purchase;
  }

  public function update($purchases) {
    if ($this->isClosed()) {
      S('log')->error('cart already ordered ('.$this->getId().')',
                        __METHOD__);
      throw new Exception(l('error.cart.invalid'));
    }
    $errors = array();
    $quantities = 0;
    foreach ($purchases as $purchase_id => $quantity) {
      $purchase = $product = null;
      try {
        $purchase = Purchase::getInstance(array('order_id' => $this->getId(),
                                                'id' => $purchase_id));
        $opts = $purchase->getOpts();
        $item = $purchase->getItem();
        if (!$item->isPurchasable()) {
          throw new Exception($item->title.' : '.l('error.product.unavailable'));
        }
        elseif ($item->getAvailability($opts) < $quantity) {
          throw new Exception($item->title.' : '.l('error.product.stock'));
        }
        $threshold = $item->getThreshold($opts);
        if ($threshold > 0) {
          $quantity = $quantity > $threshold ? $threshold : $quantity;
        }
        $purchase->setAttribute('quantity', $quantity);
      }
      catch (Exception $e) {
        S('log')->trace('purchase ('.$purchase_id.'),'.
                          ' quantity ('.$quantity.')', __METHOD__);
        if ($purchase !== null) {
          if ($item === null) {
            // TODO A VERIFIER
            // $purchase->destroy();
          }
          else {
            //$purchase->setAttribute('quantity', $item->quantity);
            $item_availability = $item->getAvailability($opts) + $purchase->quantity;
            $threshold = $item->getThreshold($opts);
            $quantity = $item_availability > $threshold
                      ? $threshold
                      : $item_availability;
            $purchase->setAttribute('quantity', $quantity);
          }
        }
        $errors[] = $e->getMessage();
      }
    }
    S('app')->hook('onCartUpdate',
                   array('order' => $this,
                         'purchases' => $purchases));
    if (count($errors) >= 1) {
      $e = new Exception();
      $e->errors = $errors;
      throw $e;
    }
  }

  public function getConversionWorth($opts) {
    return ($this->total_amount - $this->tax_amount);
  }

  public function hasExpired() {
    if (DH::isNull($this->expire_at)) return false;
    $ts = DH::asTimestamp($this->expire_at);
    return time() > $ts;
  }

  public function clear() {
    foreach ($this->getPurchases() as $purchase) {
      $purchase->destroy();
    }
    if (!DH::isNull($this->expire_at)) {
      $this->commit(array('expire_at' => DH::DATE_NULL));
    }
  }

  public function getMethod() {
    try {
      if ($this->method_id >= 1) {
        return Method::getInstance((int) $this->method_id);
      }
      $id = S('dbo')->fetchValue('SELECT p.method_id'
                                 .' FROM '.T('payment').' AS p, '
                                 .         T('order').' AS o'
                                 .' WHERE o.id='.$this->getId()
                                 .  ' AND o.id=p.order_id'
                                 .  ' AND p.status='.Payment::STATUS_RECEIVED
                               .' ORDER BY p.id DESC');
      if ($id < 1) return null;
      return Method::getInstance((int) $id);
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
      return null;
    }
  }

  public function getAmount() {
    $amount = 0;
    foreach ($this->getPurchases() as $purchase) {
      $amount += $purchase->price * $purchase->quantity * (1 + ($purchase->tax / 100));
    }
    return $amount;
  }

  public function isCompleted() {
    if (!$this->isItem()) return false;
    return $this->status == self::STATUS_PROCESSED;
  }

  public function isClosed() {
    if (!$this->isItem()) return false;
    return $this->status != self::STATUS_NONE;
  }

  public function isPaid() {
    if (!$this->isItem()) return false;
    return $this->status >= self::STATUS_PAID;
  }

  public function asEditHash() {
    $grids = array();
    $attrs = array('on' => array('click' => 'new kwo.dialogs.Preview(this)'),
                   'data' => array('url' => '/invoice?item='.$this->getKey()));
    $invoice_link = H::elt('a', $this->invoice_id, $attrs);
    $summary = array('état' => '<strong>'.strtoupper(self::$statuses[$this->status]).'</strong>',
                     'commande' => $this->getId(),
                     'facture' => $this->invoice_id >= 1 ? $invoice_link : '-',
                     'total' => $this->total_amount,
                     'livraison' => $this->shipping_amount,
                     'réduction' => $this->discount_amount,
                     'taxes' => $this->tax_amount,
                     'visit' => Visit::link($this->getMeta(Meta::FIELD_VISIT)),
                     'début' => $this->getMeta(Meta::FIELD_CREATED_AT));
    $row = array('model_id' => $this->getModelId(), 'record_id' => $this->getId());
    if (Conversion::exists($row)) {
      $summary['conversion'] = Item::link(array(Model::id('conversion'), Conversion::id($row)));
    }
    if ($this->isCompleted()) {
      $summary['envoi'] = $this->completed_at;
    }
    if ($this->handling_amount > 0) {
      $summary['manutention'] = $this->handling_amount;
    }

    if ($this->user_id >= 1) {
      try {
        $user = User::getInstance((int) $this->user_id, array('ignore' => true));
        $summary['user'] = Item::edit($user);
        $str = nl2br(h($this->billingAddress()));
        $attrs = array('data' => array('model' => 'user',
                                       'record-id' => $this->user_id,
                                       'id' => $this->id),
                       'on' => array('click' => 'editor = new kwo.dialogs.ItemEditor(this); that=this;'
                                     .' editor.onAfterClose = function () { Kwo.F(\'order\').onEdit(that); }'));
        $str .= '<br/>'.H::elt('a', 'modifier', $attrs);
      }
      catch (Exception $e) {
        $str = '** user '.$this->user_id.' **';
      }
      $summary['adr facturation'] = $str;
    }

    if ($this->addressee_id >= 1) {
      try {
        $str = nl2br(h($this->shippingAddress()));
      }
      catch (Exception $e) { $str = 'unknown'; }
      $summary['adr livraison'] = $str;
    }
    else {
      $summary['adr livraison'] = 'idem';
    }

    if ($this->warehouse_id >= 1) {
      try {
        $str = (string) Warehouse::getInstance($this->warehouse_id);
      }
      catch (Exception $e) { $str = 'unknown ('.$this->warehouse_id.')'; }
      $summary['entrepôt'] = $str;
    }

    $discounts = array();
    if ($this->discount_amount > 0) {
      try {
        $rs = $this->dbo->query('SELECT discount_id, amount'
                                .' FROM '.self::TABLE_DISCOUNT
                                .' WHERE order_id='.$this->getId());
        foreach ($rs as $row) {
          $discount = Discount::getInstance($row['discount_id']);
          $discounts[] = array('name' => $discount->name,
                               'id' => $discount->getId(),
                               'amount' => $row['amount']);
        }
      }
      catch (Exception $e) { S('log')->error($e, __METHOD__); }
    }

    $grids['summary'] = $summary;
    $grids['payments'] = $this->getPayments();
    $grids['purchases'] = $this->getPurchases();
    $grids['shipments'] = $this->getShipments();
    $grids['discounts'] = $discounts;

//    $h['grids'] = $grids;

    return array('grids' => $grids);
  }

  public function getTaxes() {
    $taxes = array();
    if ($this->tax_amount <= 0) return $taxes;
    foreach ($this->getPurchases() as $purchase) {
      if ($purchase->tax < 1) continue ;
      $tax = (string) number_format($purchase->tax, 1);
      $taxes[$tax] += $purchase->getTaxes();
      //$taxes[$tax] += ($purchase->quantity * $purchase->price * $purchase->tax) / 100;
    }
    if ($this->shipping_amount > 0 && self::SHIPPING_TAX > 0) {
      $tax = (string) number_format(self::SHIPPING_TAX, 1);
      $taxes[$tax] += ($this->shipping_amount * self::SHIPPING_TAX) / 100;
    }
    if ($this->handling_amount > 0 && self::HANDLING_TAX > 0) {
      $tax = (string) number_format(self::HANDLING_TAX, 1);
      $taxes[$tax] += ($this->handling_amount * self::HANDLING_TAX) / 100;
    }
    return $taxes;
  }

  public function checkout() {
    $this->setAttribute('checkedout_at', DH::now());
    S('app')->hook('onOrderCheckout');
  }

  protected function onBeforeStatusChange(&$attrs) {
    if ($attrs['status'] == 0) {
      throw new Exception('invalid status');
    }
  }

  public function onAfterStatusChange($old_status) {
    $status = $this->status;
    $attrs = array();
    if ($status >= self::STATUS_PENDING) {
      switch ($status) {
        case self::STATUS_PAID:
          $letter_code = 'order.paid';
          break ;
        case self::STATUS_PROCESSING:
          $letter_code = 'order.processing';
          break ;
        case self::STATUS_PROCESSED:
          $letter_code = 'order.processed';
          $attrs['completed_at'] = DH::now();
          break ;
      }
      if ($status > self::STATUS_PAID && $this->invoice_id < 1) {
        $attrs['invoice_id'] = $this->dbo->fetchValue('SELECT MAX(invoice_id) + 1'
                                                           .' FROM '.T('order'));
      }
      $user = $this->getUser();
      Base::addCustomer($user);
      if ($status >= self::STATUS_PAID && DH::isNull($this->paid_at)) {
        $attrs['paid_at'] = DH::now();
      }
      if ($old_status < self::STATUS_PAID && $status >= self::STATUS_PAID) {
        $this->convert();
        $visit_id = $this->getMeta(Meta::FIELD_VISIT);
        foreach ($this->getPayments() as $payment) {
          if ($payment->status == Payment::STATUS_PENDING) {
            $payment->setStatus(Payment::STATUS_RECEIVED);
          }
        }
        $user->setInformation('last_order_date', DH::now());
        $purchases = array();
        foreach ($this->getPurchases() as $purchase) {
          $purchases[] = $purchase;
        }
        foreach ($purchases as $purchase) {
          if ($purchase->quantity < 1) continue ;
          try {
            $item = $purchase->getItem();
            $item->onOrderPaid($this, $purchase);
            Interaction::track($item, Interaction::TYPE_PAY, array('visit_id' => $visit_id));
          }
          catch (Exception $e) { S('log')->error($e, __METHOD__); }
        }
        $order_count = $this->dbo->fetchValue('SELECT COUNT(id)'
                                              .' FROM '.T('order')
                                              .' WHERE user_id='.$user->getId()
                                              .' AND status >= '.self::STATUS_PAID);

        try {
          if ($order_count >= 1 && $user->hasSponsor()) {
            $sponsor = $user->getSponsor();
            if ($order_count == 1) {
              $sponsor->credit('godson.purchase.first',
                               array('email' => $sponsor->email));
            }
          }
        }
        catch (Exception $e) { S('log')->debug($e, __METHOD__); }

      }
    }
    if ($old_status >= self::STATUS_PAID && $status <= self::STATUS_PAID) {
      try {
        $purchases = array();
        foreach ($this->getPurchases() as $purchase) {
          $purchases[] = $purchase;
        }
        foreach ($purchases as $purchase) {
          if ($purchase->quantity < 1) continue ;
          try {
            $purchase->getItem()->onOrderCancel($this, $purchase);
          }
          catch (Exception $e) { S('log')->error($e, __METHOD__); }
        }
      }
      catch (Exception $e) { S('log')->error($e, __METHOD__); }
      Debit::remove($this);
      Conversion::remove($this);
    }
    $this->setAttributes($attrs);
    /*
    AJOUT SYLVAIN POUR FIANET
    if ($old_status < self::STATUS_PAID && $status >= self::STATUS_PAID) {
      Fianet::request($this);
    }
    */
    $this->sendMessage($letter_code);
    S('app')->hook('onOrderStatusChange',
                   array('order' => $this,
                         'old_status' => $old_status));
  }

  protected function sendMessage($code) {
    if (strlen($code) < 2) return ;
    try {
      $user = $this->getUser();
      $bindings = $this->getAttributes();
      /*      $bindings['details'] = Template::render('shop:order.details',
              array('order' => $this,
              'id' => $this->id,
              'locale' => $user->locale));*/
      $bindings['details'] = H::get('/order.details',
                                    array('id' => $this->id));
      Letter::send($code, $user, $bindings);
    }
    catch (Exception $e) { }
  }

  public function getDetails() {
    $h = array();
    return $h;
  }

  public function getAddressee() {
    if ($this->addressee_id >= 1) {
      return Addressee::getInstance($this->addressee_id);
    }
    $addressee = Addressee::getInstance();
    return $addressee;
  }

  public function getCustomer() {
    $user = $this->getUser();
    $profile = P('profile.customer') ?: P('profile.person');
    return $user->getProfile($profile);
  }

  public function billingAddress() {
    return $this->getCustomer()->asAddress();
  }

  public function getBillingAdmin2() {
    if ($this->user_id < 1) return '';
    $user = $this->getUser();
    $profile = P('profile.customer') ?: P('profile.person');
    if (!$user->hasProfile($profile)) return '';
    $profile = $user->getProfile($profile);
    return $profile->getAttribute('admin2');
  }

  public function getBillingCountryId() {
    if ($this->user_id < 1) return Country::FR;
    $user = $this->getUser();
    $profile = P('profile.customer') ?: P('profile.person');
    if (!$user->hasProfile($profile)) return Country::FR;
    $profile = $user->getProfile($profile);
    $country_id = (int) $profile->getAttribute('country_id');
    $country_id = $country_id > 0 ? $country_id : Country::FR;
    return $country_id;
  }

  public function getShippingCountryId() {
    if ($this->shipping_country_id > 0) return $this->shipping_country_id;
    if ($this->addressee_id >= 1) {
      $addressee = Addressee::getInstance($this->addressee_id);
      $country_id = $addressee->country_id;
    }
    else {
      $country_id = $this->getBillingCountryId();
    }
    return (int) ($country_id ?: Country::FR);
  }

  public function getShippingAdmin2() {
    if (!empty($this->shipping_admin2)) return $this->shipping_admin2;
    if ($this->addressee_id >= 1) {
      $addressee = Addressee::getInstance($this->addressee_id);
      return (string) $addressee->admin2;
    }
    return $this->getBillingAdmin2();;
  }

  public function shippingAddress() {
    return $this->addressee_id >= 1
         ? Addressee::getInstance($this->addressee_id)->asAddress()
         : $this->billingAddress();
  }

  private static $countries = array();

  public function hasTax() {
    try {
      $country_id = $this->getBillingCountryId();
      if (!(self::$countries[$country_id] instanceof ActiveRecord)) {
        self::$countries[$country_id] = Country::getInstance($country_id);
      }
      $country = self::$countries[$country_id];
      return $country->isTaxable();
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
      return true;
    }
  }

  public function calculate() {
    $quantity = 0;
    foreach ($this->getPurchases() as $purchase) {
      $quantity += $purchase->quantity;
    }
    if ($quantity < 1) {
      $attrs = array('purchase_amount' => 0,
                     'handling_amount' => 0,
                     'shipping_amount' => 0,
                     'tax_amount' => 0,
                     'discount_amount' => 0,
                     'total_amount' => 0);
      $this->commit($attrs);
      foreach ($this->getShipments() as $shipment) {
        $shipment->destroy();
      }
      $this->dbo->exec('DELETE FROM '.Order::TABLE_DISCOUNT
                       .    ' WHERE order_id='.$this->getId());
      return ;
    }

    Discount::check($this);
    $this->calculatePurchase();
    $this->calculateHandling();
    $this->calculateShipping();
    $this->calculateDiscount();
    $this->calculateTotal();
    $this->calculateTaxes();

    S('app')->hook('onOrderCalculate', $this);

/*    $total_amount += 0;
    $total_amount += $this->getAttribute('purchase_amount');
    $total_amount += $this->getAttribute('handling_amount');
    $total_amount += $this->getAttribute('shipping_amount');
    $total_amount += $this->getAttribute('tax_amount');
    $total_amount -= $this->getAttribute('discount_amount');

    $this->setAttribute('total_amount', $total_amount); */

    if (false) {
      $h = array('purchase' => $this->purchase_amount,
                 'handling' => $this->handling_amount,
                 'shipping' => $this->shipping_amount,
                 'tax'      => $this->tax_amount,
                 'discount' => $this->discount_amount,
                 'total'    => $this->total_amount);
      S('log')->debug($h);
    }

  }

  private function calculatePurchase() {
    if (intval($this->status) != 0) {
      throw new Exception('invalid order status');
    }
    $purchase_amount = $this->getPurchaseAmountWithoutTax();
    $attrs = array('purchase_amount' => $purchase_amount);
    $this->commit($attrs);
  }

  private function calculateHandling() {
    if ($this->isDigital() || !$this->hasFlag(self::FLAG_HANDLING)) {
      $handling_amount = 0;
    }
    else {
      $handling_amount = (float) P('shop.handling.amount');
    }
    if (P('shop.price_includes_vat') === true && self::HANDLING_TAX > 0) {
      $handling_amount = $handling_amount * 100 / (self::HANDLING_TAX + 100);
    }
    $attrs = array('handling_amount' => $handling_amount);
    $this->commit($attrs);
  }

  private function calculateShipping() {
    if ($this->isDigital()) {
      $this->setAttribute('shipping_amount', 0);
      foreach ($this->getShipments() as $shipment) {
        $shipment->destroy();
      }
      return ;
    }
    $this->dispatchShipments();
    if ($this->warehouse_id >= 1) {
      $warehouse = Warehouse::getInstance($this->warehouse_id);
      $cost = $warehouse->amount;
    }
    else {
      $cost = 0;
      foreach ($this->getShipments() as $shipment) {
        $cost += $shipment->calculateCost();
      }
    }
    if (P('shop.price_includes_vat') === true && self::SHIPPING_TAX > 0) {
      $cost = $cost * 100 / (self::SHIPPING_TAX + 100);
    }
    $this->setAttribute('shipping_amount', $cost);
  }

  public function calculateDiscount() {
    $discount_amount = (float) $this->dbo->fetchValue('SELECT SUM(amount)'
                                                      .' FROM '.self::TABLE_DISCOUNT
                                                      .' WHERE order_id='.$this->getId());
    $attrs = array('discount_amount' => $discount_amount);
    $this->commit($attrs);
  }

  private function calculateTotal() {
    $total_amount  = $this->purchase_amount;
    $total_amount += $this->shipping_amount;
    $total_amount += $this->handling_amount;
    if ($this->hasTax()) {
      foreach ($this->getPurchases() as $purchase) {
        $total_amount += $purchase->getTaxes();
      }
      $total_amount += ($this->shipping_amount * self::SHIPPING_TAX) / 100;
      $total_amount += ($this->handling_amount * self::HANDLING_TAX) / 100;
    }
    $total_amount -= $this->discount_amount;
    $attrs = array('total_amount' => $total_amount);
    $this->commit($attrs);
  }

  private function calculateTaxes() {
    if (!$this->hasTax()) {
      $this->setAttribute('tax_amount', 0);
      return ;
    }
    $tax_amount = $this->total_amount -
                 ($this->purchase_amount +
                  $this->shipping_amount +
                  $this->handling_amount -
                  $this->discount_amount);
    $attrs = array('tax_amount' => $tax_amount);
    $this->commit($attrs);
  }

  private function dispatchShipments() {
    $parcels = $shipments = array();
    foreach ($this->getPurchases() as $purchase) {
      if ($purchase->hasFlag(Purchase::FLAG_DIGITAL)) continue ;
      $parcels[$purchase->parcel_type][] = $purchase;
    }
    foreach ($parcels as $type => $purchases) {
      $shipment_id = $this->dbo->fetchValue('SELECT id'
                                            .' FROM '.T('shipment')
                                            .' WHERE parcel_type='.$type
                                            .  ' AND order_id='.$this->getId()
                                            .' LIMIT 1');
      $args = $shipment_id >= 1 ? $shipment_id : true;
      $shipment = Shipment::getInstance($args);
      $shipment->commit(array('order_id' => $this->getId(),
                              'parcel_type' => $type));
      $shipments[] = $shipment->getId();
      foreach ($purchases as $purchase) {
        if ($purchase->shipment_id == $shipment->getId()) continue ;
        $purchase->setAttribute('shipment_id', $shipment->getId());
      }
    }
    if (count($shipments) < 1) return ;
    $set = $this->dbo->asSet('SELECT id'
                            .' FROM '.T('shipment')
                            .' WHERE id NOT IN ('.join(',', $shipments).')'
                            .  ' AND order_id='.$this->getId());
    foreach ($set as $id) {
      $shipment = Shipment::getInstance($id);
      $shipment->destroy();
    }
  }

  public static function profileAddress($profile, $locale=null) {
    $ret = '';
    $ret .= c($profile['first_name']).' '.c($profile['last_name'])."\n";
    $ret .= $profile['address']."\n";
    $ret .= $profile['postal_code'].' '.strtoupper($profile['city'])."\n";
    $ret .= strtoupper(H::country($profile['country_id'], $locale));
    if (strlen($profile['phone']) >= 6) {
      $ret .= "\n".$profile['phone'];
    }
    return $ret;
  }

  public static function isValidProfile($h) {
    $h = $h instanceof ActiveRecord ? $h->getAttributes() : $h;
    $errors = array();
    if (strlen($h['first_name']) < 2) $errors[] = 'first_name';
    if (strlen($h['last_name']) < 2) $errors[] = 'last_name';
    if (strlen($h['address']) < 3) $errors[] = 'address';
    if (strlen($h['postal_code']) < 2) $errors[] = 'postal_code';
    if (strlen($h['city']) < 2) $errors[] = 'city';
    if ($h['country_id'] < 1) $errors[] = 'country_id';
    if (count($errors) < 1) {
      return true;
    }
    //    S('log')->debug($errors, __METHOD__);
    return false;
  }

  public static function load($autocreate=false) {
    $req = S('req');
    $cookie = $req->getCookie();
    $order_id = (int) $cookie->getAttribute('order_id');
    if ($order_id >= 1 && Order::exists($order_id)) {
      $order = Order::getInstance($order_id);
      if ($order->isClosed()) {
        $order->markAsEvolved();
        $order = null;
      }
    }
    elseif ($autocreate === false) {
      throw new OrderException();
    }
    if (!($order instanceof Order)) {
      $order = Order::getInstance();
      $attrs = array('comment' => '',
                     'merchant_id' => Merchant::getInstanceByApp()->getId());
      try {
        $carrier = Carrier::getDefault();
        if ($carrier) {
          $attrs['carrier_id'] = $carrier->getId();
        }
      }
      catch (Exception $e) {
        S('log')->error($e, __METHOD__);
      }
      if ($cookie->hasAttribute('shipping_country_id')) {
        $attrs['shipping_country_id'] = (int) $cookie->getAttribute('shipping_country_id');
      }
      $order->commit($attrs);
      $order->setVisit($req);
      $cookie->setAttribute('order_id', $order->getId());
    }
    if ($order->shipping_country_id < 1) {
      $order->setAttribute('shipping_country_id', $req->getClient('country'));
    }
    if ($order->user_id < 1 && $req->isAuthenticated()) {
      $order->setUser($req->getUser());
    }
    S('app')->hook('onOrderLoad', array('order' => $order));
    S('res')->disableCache();
    return $order;
  }

  public static $codes = array(self::STATUS_PROCESSED => 'processed',
                               self::STATUS_PAID => 'paid',
                               self::STATUS_PROCESSING => 'processing',
                               self::STATUS_PENDING => 'pending',
                               self::STATUS_CANCELLED => 'cancelled',
                               self::STATUS_ERROR => 'error');
}
