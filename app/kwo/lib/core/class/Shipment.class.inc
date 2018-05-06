<?php

class Shipment extends ActiveRecord {

  const STATUS_PENDING = 1;
  const STATUS_PROCESSING = 2;
  const STATUS_PROCESSED = 3;

  const UNKNOWN_AMOUNT = 100;

  public function calculateCost($carrier_id=null) {
    $order = $this->getOrder();
    $volume = $weight = $amount = 0;
    foreach ($this->getPurchases() as $purchase) {
      if ($purchase->quantity < 1) continue ;
      $item = $purchase->getItem();
      $volume += (float) ($item->volume * $purchase->quantity);
      $weight += (float) ($item->weight * $purchase->quantity);
      $amount += (float) ($purchase->getPriceWithoutTax() * $purchase->quantity);
    }
    $country_id = $order->getShippingCountryId();
    $admin2 = $order->getShippingAdmin2();
    if ($country_id < 1) {
      $country_id = Country::FR;
    }
    if (strlen($admin2) == 2) {
      $where .= ' AND (r.admin2="'.$admin2.'" OR r.admin2="")';
    }
    else {
      $where .= ' AND r.admin2="'.$admin2.'"';
    }
    $where .= ' AND r.country_id='.$country_id;
    if ($carrier_id >= 1 || $order->carrier_id >= 1) {
      $where .= ' AND c.id='.($carrier_id ?: $order->carrier_id);
    }
    $where .= ' AND (c.app ="" OR c.app="'.S('app')->getName().'")';
    $where .= ' AND '.$volume.' BETWEEN volume_min AND volume_max'
             .' AND '.$weight.' BETWEEN weight_min AND weight_max'
             .' AND '.$amount.' BETWEEN amount_min AND amount_max ';
    $h = $this->dbo->fetchHash('SELECT c.id AS carrier_id, r.amount'
                               .' FROM '.T('carrier').' AS c,'
                               .     ' '.Carrier::TABLE_RATES.' AS r'
                               .' WHERE c.status >= 1'
                               .  ' AND r.carrier_id=c.id'.$where
                               .' ORDER BY r.admin2 DESC,'
                               .         ' volume_min ASC,'
                               .         ' weight_min DESC');

    /*                                         .' ORDER BY c.priority ASC,'
                                               .         ' r.admin2 DESC,'
                                               .         ' volume_min ASC,'
                                               .         ' weight_min DESC');*/
    //$this->dbo->debug();
    //S('log')->debug($h);
    if (empty($h)) {
//      $this->dbo->debug();
//      S('log')->trace('unknown shipping amount', __METHOD__);
      return self::UNKNOWN_AMOUNT;
    }
    if ($carrier_id < 1) {
      $this->commit($h);
    }
    /*    S('log')->debug(array('weight' => $weight,
                          'volume' => $volume,
                          'amount' => $h['amount'],
                          'carrier_id' => $h['carrier_id']));*/
    return $h['amount'];
  }

  public function getVolume() {}

  public function getWeight() {}

  public function getAddressee() {
    $order = $this->getOrder();
    if ($order->addressee_id >= 1) return $order->getAddressee();
    return $order->getCustomer();
  }

  public function asListHash() {
    try {
      $h = array();
      if ($this->carrier_id >= 1) {
        $carrier = $this->getCarrier();
        $h['name'] = $carrier->getName();
        if ($carrier->hasFlag(Carrier::FLAG_POSTAL)) {
          $h['messagerie'] = 'oui';
        }
      }
      $h['montant'] = $this->amount;
      $h['produits'] = '<br/>';
      foreach ($this->getPurchases() as $purchase) {
        $h['produits'] .= ' &middot; '.$purchase->getName().'<br/>';
      }
      return $h;
    }
    catch (Exception $e) {
      return '** shipment '.$this->id.' **';
    }
  }


}