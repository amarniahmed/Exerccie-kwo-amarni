<?php

class Movement extends ActiveRecord {

  const FLAG_SPONSORSHIP = 1;

  public static $flags = array(self::FLAG_SPONSORSHIP => 'parrainage');

  public static function trigger($user, $code, $bindings=null) {
    if (!Movement::exists($code)) return ;
    $movement = Movement::getInstance($code);
    if (!$movement->isOnline()) return ;
    $credit = Credit::getInstance();
    $expire_on =  date(DH::DATE_MYSQL,
                       time() + (3600 * 24 * $movement->duration));
    $attrs = array('amount' => $movement->amount,
                   'expire_on' => $expire_on,
                   'movement_id' => $movement->getId(),
                   'user' => $user->getId(),
                   'name' => SH::bind($movement->label, $bindings));
    try {
      if ($movement->hasFlag(self::FLAG_SPONSORSHIP) &&
          $user->hasSponsor()) {
        $attrs['sponsor_id'] = $user->getSponsorId();
      }
    }
    catch (Exception $e) { S('log')->error($e, __METHOD__); }
    $credit->commit($attrs);
    $credit->setUser($user);
  }

}