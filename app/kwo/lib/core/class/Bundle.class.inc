<?php

class Bundle extends ActiveRecord {
  
  public function getBundleItems() {
    $rs = $this->dbo->query('SELECT *'
                            .' FROM core_item_bundle'
                            .' WHERE bundle_id='.$this->getId());
    $items = array();
    foreach ($rs as $hash) {
      $items[] = Item::getInstance($hash['model_id'], 
                                   $hash['record_id']);
    }
    return $items;
  }

  public function getPrice() {
    $price = 0;
    foreach ($this->getBundleItems() as $item) {
      $price += $item->getPrice();
    }
    return $price;
  }

  public function purchase($cart) {
    $rs = $this->dbo->query('SELECT *'
                            .' FROM core_item_bundle'
                            .' WHERE bundle_id='.$this->getId());
    $purchases = array();
    try {
      foreach ($rs as $hash) {
        $item = Item::getInstance($hash['model_id'], 
                                  $hash['record_id']);
        if (!$item->isPurchasable()) {
          throw new Exception(l('error.product.unavailable'));
        }
        $purchases[] = $cart->addPurchase($item, 
                                          array('bundle_id' => $this->getId()));
      }
    }
    catch (Exception $e) {
      foreach ($purchases as $purchase) {
        $purchase->destroy();
      }
      throw $e;
    }
  }

}