<?php

class Transaction extends ActiveRecord {

  public function getAmount() {
    return $this->amount;
  }

}