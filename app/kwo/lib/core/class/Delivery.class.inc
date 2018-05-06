<?php

class Delivery extends ActiveRecord {

  public static function markAsClicked($id, $url) {
    $id = (int) $id;
    if (!Delivery::exists($id)) return ;
    $delivery = Delivery::getInstance($id);
    $attrs = array('clicked_at' => DH::now());
    if (strlen($delivery->clicks) > 3) {
      $clicks = unserialize($delivery->clicks);
    }
    else {
      $clicks = array();
    }
    $clicks[DH::now()] = $url;
    $clicks = serialize($clicks);
    $attrs['clicks'] = $clicks;
    $delivery->setAttributes($attrs);
    if (DH::isNull($delivery->opened_at)) {
      self::markAsOpened($id);
    }
  }

  public static function markAsOpened($id) {
    $id = (int) $id;
    if (!Delivery::exists($id)) return ;
    $delivery = Delivery::getInstance($id);
    if (!DH::isNull($delivery->opened_at)) return ;
    $attrs = array('opened_at' => DH::now());
    $delivery->setAttributes($attrs);
    if ($delivery->user_id > 0) {
      $user = User::getInstance($delivery->user_id);
      if ($user->status == User::STATUS_PENDING) {
        $user->setStatus(User::STATUS_CONFIRMED);
      }
    }
  }

}