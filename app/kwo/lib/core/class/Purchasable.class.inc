<?php

interface Purchasable 
{
  /* code name description price tax image */
  public function asPurchaseHash($order, $opts);

  public function onOrderPaid($order, $purchase);

  public function onOrderCancel($order, $purchase);

  public function onPurchase($purchase);

  public function isDigital();

  public function isPurchasable();

  public function getPrice();

  public function getAvailability($opts);
  
  public function getThreshold($opts);

}