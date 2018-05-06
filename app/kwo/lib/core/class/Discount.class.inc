<?php

class Discount extends ActiveRecord {

  const TABLE_ACTION = 'core_discount_action';
  const TABLE_CONDITION = 'core_discount_condition';

  const FLAG_BLOCKING = 1;
  const FLAG_SINGLE_USE = 2;

  public static $flags = array(self::FLAG_BLOCKING => 'bloquante',
                               self::FLAG_SINGLE_USE => 'utilisation unique');

  const STATUS_ONLINE = 10;
  const STATUS_PENDING = 0;
  const STATUS_OFFLINE = -10;
  const STATUS_REMOVED = -100;

  public static $statuses = array(self::STATUS_ONLINE => 'active',
                                  self::STATUS_PENDING => 'planifiée',
                                  self::STATUS_OFFLINE => 'inactive',
                                  self::STATUS_REMOVED => 'supprimée');
  const TYPE_CATALOG = 1;
  const TYPE_CART = 2;

  public static $types = array(self::TYPE_CATALOG => 'catalogue',
                               self::TYPE_CART => 'panier');

  const ACTION_ATTRIBUTE_AMOUNT = 1;
  const ACTION_ATTRIBUTE_SHPPING = 2;

  const ACTION_VALUE_TYPE_FIXED = 1;
  const ACTION_VALUE_TYPE_PERCENT = 2;

  public static $action_value_types = array(self::ACTION_VALUE_TYPE_FIXED => 'Eur',
                                            self::ACTION_VALUE_TYPE_PERCENT => '%');


  public function asEditHash() {
    $h = array('composer' => '');
    if ($this->isItem()) {
      $h['composer'] .= H::get('/back/core/discount.composer',
                                array('id' => $this->getId()));
      $h['composer'] .= H::trigger('new kwo.managers.Discount(this)');
    }
    return $h;
  }

  public function asFinderHash($h) {
    $h['name'] .= $this->isBlocking() ? '*' : '';
    return $h;
  }

  public function destroy() {
    if (S('req')->isCli()) {
      parent::destroy();
      return ;
    }
    if ($this->type == self::TYPE_CART) {
      $table = S('dbo')->getTable(Order::TABLE_DISCOUNT);
      $row = array('discount_id' => $this->getId());
      if ($table->hasRow($row)) {
        $this->setAttribute('status', self::STATUS_REMOVED);
      }
      else {
        parent::destroy();
      }
    }
    else {
      $this->cancel();
      $table = S('dbo')->getTable(Purchase::TABLE_DISCOUNT);
      $row = array('discount_id' => $this->getId());
      if ($table->hasRow($row)) {
        $this->setAttribute('status', self::STATUS_REMOVED);
      }
      else {
        parent::destroy();
      }
    }
  }

  protected function onAfterDestroy() {
    $this->dbo->exec('DELETE FROM '.self::TABLE_CONDITION
                     .    ' WHERE discount_id='.$this->getId());
    $this->dbo->exec('DELETE FROM '.self::TABLE_ACTION
                     .    ' WHERE discount_id='.$this->getId());
    $this->dbo->exec('DELETE FROM '.Order::TABLE_DISCOUNT
                     .    ' WHERE discount_id='.$this->getId());
    $this->dbo->exec('DELETE FROM '.Purchase::TABLE_DISCOUNT
                     .    ' WHERE discount_id='.$this->getId());
  }

  protected function onAfterStatusChange($old) {
    if ($this->isCart()) return ;
    if ($old > self::STATUS_PENDING && $this->status <= self::STATUS_PENDING) {
      $this->cancel();
    }
    elseif ($old < self::STATUS_ONLINE && $this->status >= self::STATUS_ONLINE) {
      $this->apply();
    }
  }

  public function isBlocking() { return $this->hasFlag(self::FLAG_BLOCKING); }
  public function isCatalog() { return $this->isItem() && $this->type == self::TYPE_CATALOG; }
  public function isCart() { return $this->isItem() && $this->type == self::TYPE_CART; }

  public function cancel() {
    $model_id = (int) $this->dbo->fetchValue('SELECT model_id'
                                             .' FROM '.self::TABLE_CONDITION
                                             .' WHERE discount_id='.$this->getId()
                                             .' LIMIT 1');
    if ($model_id < 0) return ;
    $rs = $this->dbo->query('SELECT record_id, amount'
                            .' FROM '.Item::TABLE_DISCOUNT
                            .' WHERE model_id='.$model_id
                            .  ' AND discount_id='.$this->getId());
    foreach ($rs as $row) {
      $item = Item::getInstance($model_id, (int) $row['record_id']);
      $price = $item->getAttribute('price') + $row['amount'];
      $price = $price > 0 ? $price : 0;
      $item->setAttribute('price', $price);
    }
    $this->dbo->exec('DELETE FROM '.Item::TABLE_DISCOUNT
                     .    ' WHERE model_id='.$model_id
                     .      ' AND discount_id='.$this->getId());
  }

  public function apply($item=null) {
    if (!$this->isCatalog()) {
      throw new Exception('seule une règle catalogue peut être appliquée');
    }
    if ($this->status != self::STATUS_ONLINE) {
      return $this->cancel();
    }
    $conditions = $this->getConditions();
    if ($item instanceof ActiveRecord) {
      $items = array($item);
    }
    else {
      $model_id = $this->dbo->fetchValue('SELECT model_id'
                                         .' FROM '.self::TABLE_CONDITION
                                         .' WHERE discount_id='.$this->getId()
                                         .' LIMIT 1');
      $items = Item::collect($model_id);
    }
    $action = $this->getAction();
    $is_percent = false;
    $rate = $action['rate'];
    if (strpos($rate, '%') !== false) {
      $is_percent = true;
      $rate = trim($rate, '%');
    }
    foreach ($items as $item) {
      $this->dbo->exec('DELETE FROM '.Item::TABLE_DISCOUNT
                       .    ' WHERE model_id='.$item->getModelId()
                       .      ' AND record_id='.$item->getId()
                       .      ' AND discount_id='.$this->getId());
      $original_price = $item->getAttribute('original_price');
      if ($original_price <= 0) {
        $original_price = $item->getAttribute('price');
      }
      foreach ($conditions as $condition) {
        $attr = $condition['attribute'];
        $matched = true;
        if ($attr == 'price') {
          $matched = $item->getAttribute('price') >= $condition['value'];
        }
        elseif ($attr == 'tag') {
          $value = Tag::getByItem($item);
          $values = SH::asSet($condition['value']);
          $matched = in_array($value, $values);
        }
        elseif (method_exists($item, 'matchDiscountCondition')) {
          $matched = (bool) $item->matchDiscountCondition($condition);
        }
        elseif (strpos($condition['value'], ',') > 0) {
          $values = SH::asSet($condition['value']);
          $matched = in_array($value, $values);
        }
        else {
          $value = $item->getAttribute($attr);
          $matched = $value == $condition['value'];
        }
        if ($matched === false) break ;
        if ($is_percent) {
          $amount = ($original_price * $rate) / 100;
        }
        else {
          $amount = $rate;
        }
        $this->dbo->insert(Item::TABLE_DISCOUNT,
                           array('model_id' => $item->getModelId(),
                                 'record_id' => $item->getId(),
                                 'discount_id' => $this->getId(),
                                 'amount' => $amount));
      }
      $amount = $this->dbo->fetchValue('SELECT SUM(amount)'
                                       .' FROM '.Item::TABLE_DISCOUNT
                                       .' WHERE model_id='.$item->getModelId()
                                       .  ' AND record_id='.$item->getId());
      $price = $original_price - $amount;
      $price = $price <= 0 ? 0 : $price;
      $item->setAttributes(array('price' => $price,
                                 'original_price' => $original_price));
    }
  }

  private function conditionMatchOrder($condition, $order, $purchases) {
    if ($condition['attribute'] == 'quantity') {
      return $order->getPurchaseCount() >= $condition['value'];
    }
    if ($condition['attribute'] == 'amount') {
      return $order->getPurchaseAmountWithoutTax() >= $condition['value'];
    }
    if ($condition['attribute'] == 'coupon') {
      return strtolower($condition['value']) == strtolower($order->coupon);
    }
    if ($condition['attribute'] == 'user') {
      $set = SH::asSet($condition['value']);
      if (strpos($condition['value'], '@') > 0) {
        try {
          $user = User::getInstance((int) $order->user_id);
          return in_array($user->email, $set);
        }
        catch (Exception $e) { return false; }
      }
      return in_array($order->user_id, $set);
    }
    return false;
  }

  private function conditionMatchPurchases($condition, $order, $purchases) {
    $has_tax = $order->hasTax();
    foreach ($purchases as $purchase) {
      if (self::matchItem($purchase, $condition) === true) {
        if (!empty($condition['threshold_type']) && $condition['threshold'] > 0) {
          if ($condition['threshold_type'] == 'amount') {
            $price = $has_tax
                   ? $purchase->getPriceWithTax()
                   : $purchase->getPriceWithoutTax();
            $amount = $price * $purchase->quantity;
            if ($condition['threshold'] > $amount) continue ;
          }
          elseif ($condition['threshold_type'] == 'quantity') {
            if ($condition['threshold'] > $purchase->quantity) continue ;
          }
        }
        return true;
      }
    }
    return false;
  }

  private function conditionMatch($condition, $order, $purchases) {
    if ($condition['model_id'] == Model::id('order')) {
      return $this->conditionMatchOrder($condition, $order, $purchases);
    }
    else {
      return $this->conditionMatchPurchases($condition, $order, $purchases);
    }
  }

  public function match($order, $purchases=null) {
    if ($purchases === null) {
      $purchases = $order->getPurchases();
    }
    $dbo = $this->dbo;
    if ($this->hasFlag(self::FLAG_SINGLE_USE) && $this->isUsedByUser($order->user_id, $order)) {
      $this->trace('single use');
      return false;
    }
    if ($this->quota >= 1 && $this->getUseCount() > $this->quota) {
      $this->trace('over quota');
      return false;
    }
    $conditions = $this->getConditions();
    if ($conditions->count() < 1) return false;
    foreach ($conditions as $condition) {
      $this->trace($condition);
      if (!$this->conditionMatch($condition, $order, $purchases)) {
//        d($condition);
        $this->trace('failure');
        return false;
      }
    }
    return true;
  }

  public function isUsedByUser($user, $order) {
    $user_id = $user instanceof User ? $user->getId() : (int) $user;
    $n = $this->dbo->fetchValue('SELECT COUNT(o.id)'
                                .' FROM '.Order::TABLE_DISCOUNT.' AS d,'
                                .     ' '.T('order').' AS o'
                                .' WHERE d.discount_id='.$this->getId()
                                .  ' AND d.order_id=o.id'
                                .  ' AND o.id<>'.$order->getId()
                                .  ' AND o.user_id='.$user_id
                                .  ' AND o.status>='.Order::STATUS_PENDING);
    return $n >= 1;
  }

  public function getUseCount() {
    $n = $this->dbo->fetchValue('SELECT COUNT(o.id)'
                                .' FROM '.Order::TABLE_DISCOUNT.' AS d,'
                                .     ' '.T('order').' AS o'
                                .' WHERE d.discount_id='.$this->getId()
                                .  ' AND d.order_id=o.id'
                                .  ' AND o.status>='.Order::STATUS_PENDING);
    return $n;
  }

  public function getAction() {
    $action = $this->dbo->fetchHash('SELECT *'
                                    .' FROM '.Discount::TABLE_ACTION
                                    .' WHERE discount_id='.$this->getId());
    return $action;
  }

  public function getConditions() {
    $rs = $this->dbo->query('SELECT *'
                            .' FROM '.Discount::TABLE_CONDITION
                            .' WHERE discount_id='.$this->getId());
    return $rs;
  }

  public function applyAction($order, $purchases) {
    $action = $this->getAction();
    if (empty($action)) return ;
    $has_tax = $order->hasTax();
    $amount = 0;
    $quantity = 0;
    $threshold = $action['threshold'];
    $threshold_type = $action['threshold_type'] === 'quantity' ? 'quantity' : 'amount';
    $rate = $action['rate'];
    $is_percent = false;
    if (strpos($rate, '%') !== false) {
      $is_percent = true;
      $rate = trim($rate, ' %');
    }
    if ($action['model_id'] == Model::id('order')) {
      if (!$is_percent) {
        $amount = $rate;
      }
      else {
        if ($action['attribute'] == 'shipping') {
          $amount = $order->getShippingAmount();
          if ($has_tax) {
            $amount += ($amount * Order::SHIPPING_TAX) / 100;
          }
        }
        else {
          if ($has_tax) {
            $amount = $order->getPurchaseAmountWithTax();
          }
          else {
            $amount = $order->getPurchaseAmountWithoutTax();
          }
        }
        $amount = ($amount * $rate) / 100;
      }
    }
    else {
      foreach ($purchases as $purchase) {
        if (!self::matchItem($purchase, $action)) continue ;
        for ($i = 1; $i <= $purchase->quantity; $i++) {
          if ($threshold > 0) {
            if ($threshold_type === 'quantity' && $quantity >= $threshold) break ;
            if ($threshold_type === 'amount' && $amount >= $threshold) break ;
          }
          $quantity++;
          if ($rate == 'calc') {
            $item = $purchase->getItem();
            if (method_exists($item, 'getDiscountAmount')) {
              $amount = $item->getDiscountAmount($purchase, $this);
            }
          }
          elseif ($is_percent) {
            $price = $has_tax
                   ? $purchase->getPriceWithTax()
                   : $purchase->getPriceWithoutTax();
            $amount += ($price * $rate) / 100;
          }
          else {
            $amount += $rate;
          }
        }
      }
    }
    if ($threshold > 0 && $threshold_type === 'amount') {
      $amount = $threshold;
    }
    if ($amount > 0) {
      $this->trace('montant réduction : '.$amount);
      $this->dbo->insert(Order::TABLE_DISCOUNT,
                         array('discount_id' => $this->getId(),
                               'order_id' => $order->getId(),
                               'amount' => $amount));
    }
  }

  private function trace($msg) {
    return ;
    S('log')->debug($msg, 'discount['.$this->name.']');
  }

  public function isAvailableForUser($user) {
    $rs = S('dbo')->query('SELECT c.value'
                          .' FROM '.self::TABLE_CONDITION.' AS c'
                          .' WHERE discount_id='.$this->getId()
                          .  ' AND attribute="user"');
    if ($rs->count() < 1) return true;
    foreach ($rs as $row) {
      $values = SH::asSet(strtolower($row['value']));
      if (in_array($user->getId(), $values) !== false) continue ;
      if (in_array($user->email, $values) !== false) continue ;
      return false;
    }
    return true;
  }

  public function getCoupon() {
    return S('dbo')->fetchValue('SELECT value'
                                .' FROM '.self::TABLE_CONDITION
                                .' WHERE discount_id='.$this->getId()
                                .  ' AND attribute="coupon"');
  }

  public static function applyCatalogDiscounts($item) {
    $filters = array('type' => self::TYPE_CATALOG,
                     'status' => self::STATUS_ONLINE);
    $opts = array('filters' => $filters,
                  'order' => array('priority' => 'desc'));
    foreach (Discount::collect($opts) as $discount) {
      try {
        $discount->apply($item);
      }
      catch (Exception $e) {
        S('log')->error($e, __METHOD__);
      }
    }
  }

  private static function matchItem($item, $condition) {
    $purchase = null;
    if ($item->isA('purchase')) {
      $purchase = null;
      $item = $item->getItem();
    }
    if ($condition['model_id'] != $item->getModelId()) {
      return false ;
    }
    $attr = $condition['attribute'];
    if ($attr == 'id') {
      $value = $item->getId();
    }
    elseif ($attr == 'price') {
      $value = $item->getAttribute('price');
    }
    elseif ($attr == 'tag') {
      $value = Tag::getByItem($item);
    }
    elseif (method_exists($item, 'matchDiscountCondition')) {
      $value = (bool) $item->matchDiscountCondition($condition, $purchase);
    }
    else {
      $value = $item->getAttribute($attr);
    }
//    $this->trace(Model::name($item).'.'.$attr.'.'.$condition['value'].' ? '.$value);
    if (is_bool($value)) {
      return $value;
    }
    if ($attr == 'tag') {
      return in_array($condition['value'], $value);
    }
    if (strpos($condition['value'], ',') > 0) {
      $values = SH::asSet($condition['value']);
      return in_array($value, $values);
    }
    if ($attr == 'price') {
      return $value >= $condition['value'];
    }
    return $value == $condition['value'];
  }

  public static function collectByUser($user) {
    $dbo = S('dbo');
    $rs = $dbo->query('SELECT d.id'
                      .' FROM '.T('discount').' AS d,'
                      .     ' '.self::TABLE_CONDITION.' AS c'
                      .' WHERE c.attribute="coupon"'
                      .  ' AND c.discount_id=d.id'
                      .  ' AND d.status='.self::STATUS_ONLINE
                      .' ORDER BY d.available_to ASC');
    $discounts = array();
    foreach ($rs as $row) {
      $discount = Discount::getInstance((int) $row['id']);
      if (!$discount->isAvailableForUser($user)) continue;
      $discounts[] = $discount;
    }
    return $discounts;
  }

  public static function check($order) {
    try {
      $opts = array('order' => array('price' => 'desc'));
      $purchases = $order->getPurchases($opts);
      S('dbo')->exec('DELETE FROM '.Order::TABLE_DISCOUNT
                     .    ' WHERE order_id='.$order->getId());
      if ($purchases->count() < 1) return ;
      $filters = array('type' => self::TYPE_CART,
                       'status' => self::STATUS_ONLINE);
      $opts = array('filters' => $filters,
                    'order' => array('priority' => 'desc'));
      foreach (Discount::collect($opts) as $discount) {
        $discount->trace('order : '.$order->id);
        if ($discount->match($order, $purchases)) {
          $discount->trace('MATCHED');
          $discount->applyAction($order, $purchases);
          if ($discount->hasFlag(self::FLAG_BLOCKING)) {
            $discount->trace('BLOCKING');
            break ;
          }
        }
      }
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
    }
  }

  public static function collectByItem($item) {
    try {
      $set = array();
      $req = S('req');
      if ($req->hasCookie() && $req->getCookie()->getAttribute('order_id') > 0) {
        $order_id = (int) $req->getCookie()->getAttribute('order_id');
        $set = S('dbo')->asSet('SELECT discount_id'
                               .' FROM '.Order::TABLE_DISCOUNT
                               .' WHERE order_id='.$order_id);
      }
      $set[] = 0;
      $rs = S('dbo')->query('SELECT c.*'
                            .' FROM '.self::TABLE_CONDITION.' AS c,'
                            .     ' '.T('discount').' AS d'
                            .' WHERE c.model_id='.$item->getModelId()
                            .  ' AND c.discount_id=d.id'
                            .  ' AND NOT (c.discount_id IN ('.join(',', $set).'))'
                            .  ' AND d.type='.self::TYPE_CART
                            .  ' AND d.status='.Discount::STATUS_ONLINE
                            .' ORDER BY position DESC');
      $discounts = array();
      foreach ($rs as $row) {
        $discounts[$row['discount_id']][] = $row;
      }
      if (empty($discounts)) return Discount::collect(false);
      $set = array();
      foreach ($discounts as $discount_id => $conditions) {
        $matched = true;
        foreach ($conditions as $condition) {
          if (!self::matchItem($item, $condition)) {
//            S('log')->debug($condition, 'failure');
            $matched = false;
          }
        }
        if ($matched == true) {
          $set[] = $discount_id;
        }
      }
      if (empty($set)) return Discount::collect(false);
      $opts = array('filters' => array('id' => $set));
      $discounts = Discount::collect($opts);
      return $discounts;
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
      return Discount::collect(false);
    }
  }

}
