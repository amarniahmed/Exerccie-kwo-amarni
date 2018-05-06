<?php

class Credit extends ActiveRecord {

  const FLAG_GIFT = 1;

  public static $flags = array(self::FLAG_GIFT => 'cadeau');


  public static function trigger($user, $amount, $label, $opts=null, $bindings=null, $item=null) {
    if ($amount <= 0) return ;
    $credit = Credit::getInstance();
    $attributes = array('amount' => $amount,
                        'expire_on' => Dh::toDb($opts['expire_on']),
                        'flags' => (int) $opts['flags'],
                        'movement_id' => (int) $opts['movement_id'],
                        'name' => SH::bind($label, $bindings),
                        'order_id' => (int) $opts['order_id'],
                        'sponsor_id' => (int) $opts['sponsor_id'],
                        'user_id' => $user->getId());
    if ($item instanceof ActiveRecord) {
      $attributes['model_id'] = $item->getModelId();
      $attributes['record_id'] = $item->getId();
    }
    $credit->commit($attributes);
    $credit->setUser($user);
  }

  public static function remove($item, $args=null) {
    $opts = array('filters' => array('model_id' => $item->getModelId(),
                                     'record_id' => $item->getId()));
    foreach ((array) $args as $key => $value) {
      if (Model::hasField(Model::id('credit'), $key)) {
        $opts['filters'][$key] = $value;
      }
    }

    foreach (Credit::collect($opts) as $credit) {
      $credit->destroy();
    }
    /*S('dbo')->exec('DELETE FROM '.T('debit')
                    .' WHERE model_id='.$item->getModelId()
                    .  ' AND record_id='.$item->getId());*/
  }

}