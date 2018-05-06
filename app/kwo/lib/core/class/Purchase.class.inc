<?php

class Purchase extends ActiveRecord {

  const TABLE_DISCOUNT = 'core_discount_purchase';

  const STATUS_RETURNED = -1;
  const STATUS_OK = 1;

  const FLAG_DIGITAL = 1;
  const FLAG_DISCOUNT = 2;
  
  public static $flags = array(self::FLAG_DIGITAL => 'digital',
                               self::FLAG_DISCOUNT => 'réduction');

  public function isDigital() {
    return $this->hasFlag(self::FLAG_DIGITAL);
  }

  public function hasDiscount() {
    return $this->hasFlag(self::FLAG_DISCOUNT);
  }

  public function remove() {
    if (!$this->isItem()) return ;
    $this->commit(array('quantity' => 0));
  }

  public function getOpts() {
    return array('variations' => $this->variations,
                 'quantity' => $this->quantity,
                 'pack_id' => $this->pack_id);
  }

  public function getPrice() {
    return $this->price;
  }

  public function getTaxes() {
    $taxes = $this->getPriceWithTax() - $this->getPriceWithoutTax();
    $taxes = $this->quantity * $taxes;
    return $taxes;
  }

  public function getPriceWithoutTax() {
    $price = $this->price;
    if (P('shop.price_includes_vat') === true) {
      $price = $price * 100 / ($this->tax + 100);
    }
    return $price;
  }

  public function getPriceWithTax() {
    $price = $this->price;
    if (P('shop.price_includes_vat') !== true) {
      $price = $price + ($price * $this->tax / 100);
    }
    return $price;
  }

  public function asListHash() {
    try {
      $item = $this->getItem();
      $ht['name'] = $item->getName();
      $ht['price'] = $this->price.' EUR';
      $ht['tax'] = $this->tax.'%';
      $ht['type'] = Item::edit($item, array('view' => 'item'));
      if (!empty($item->code)) {
        $ht['code'] = $item->code;
      }
      $ht['qty'] = $this->quantity;
      $ht['heure'] = $this->getMeta(Meta::FIELD_CREATED_AT);
      try {
        $rs = $this->dbo->query('SELECT * FROM '.self::TABLE_DISCOUNT
                                .      ' WHERE purchase_id='.$this->getId());
        $discounts = array();
        foreach ($rs as $row) {
          $discount_id = (int) $row['discount_id'];
          $discounts[] = Discount::getInstance($discount_id).
                         ' ('.$row['amount'].' EUR)';
        }
        if (!empty($discounts)) {
          $ht['réductions'] = join('<br/>', $discounts);
        }
      }
      catch (Exception $e) {
        S('log')->error($e, __METHOD__);
      }
      if (method_exists($item, 'asPurchaseListHash')) {
        $ht += $item->asPurchaseListHash($this);
      }
      return $ht;
    }
    catch (Exception $e) {
      return '** purchase '.$this->id.' : '.
             Model::name($this->model_id).' '.$this->record_id.' **';
    }
  }

  protected function onAfterDestroy() {
    $this->dbo->exec('DELETE FROM '.self::TABLE_DISCOUNT
                     .    ' WHERE purchase_id='.$this->getId());
  }

  protected function onAfterCreate() {
    $rs = $this->dbo->query('SELECT * FROM '.Item::TABLE_DISCOUNT
                            .      ' WHERE model_id='.$this->model_id
                            .        ' AND record_id='.$this->record_id);
    foreach ($rs as $row) {
      $this->dbo->insert(self::TABLE_DISCOUNT,
                         array('discount_id' => $row['discount_id'],
                               'purchase_id' => $this->getId(),
                               'amount' => $row['amount']));
    }
  }

  public function getDiscountAmount() {
    $n = (float) $this->dbo->fetchValue('SELECT SUM(amount)'
                                        .' FROM '.self::TABLE_DISCOUNT
                                        .' WHERE purchase_id='.$this->getId());
    return $n;
  }

}
