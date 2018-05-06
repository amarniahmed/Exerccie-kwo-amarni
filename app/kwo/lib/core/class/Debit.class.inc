<?php

class Debit extends ActiveRecord {

  public static function trigger($user, $amount, $label, $opts=null, $bindings=null, $item=null) {
    if ($amount <= 0) return ;
    $debit = Debit::getInstance();
    $attributes = array('amount' => $amount,
                        'user_id' => $user->getId(),
                        'name' => SH::bind($label, $bindings),
                        'order_id' => (int) $opts['order_id'],
                        'user_id' => $user->getId());
    if ($item instanceof ActiveRecord) {
      $attributes['model_id'] = $item->getModelId();
      $attributes['record_id'] = $item->getId();
    }
    $debit->commit($attributes);
    $debit->setUser($user);
  }

  public static function remove($item, $args=null) {
    $opts = array('filters' => array('model_id' => $item->getModelId(),
                                     'record_id' => $item->getId()));
    foreach ((array) $args as $key => $value) {
      if (Model::hasField(Model::id('debit'), $key)) {
        $opts['filters'][$key] = $value;
      }
    }

    foreach (Debit::collect($opts) as $debit) {
      $debit->destroy();
    }
    /*S('dbo')->exec('DELETE FROM '.T('debit')
                    .' WHERE model_id='.$item->getModelId()
                    .  ' AND record_id='.$item->getId());*/
  }

}