<?php

class Appeal extends ActiveRecord {

  const TYPE_ACL = 1;

  public static $types = array(self::TYPE_ACL => 'acl');

  const STATUS_PENDING_REVIEW = 0;
  const STATUS_ACCEPTED = 100;

  protected function onAfterStatusChange($old) {
    if ($this->status != self::STATUS_ACCEPTED) return ;
    try {
      $item = $this->getItem();
      $user = $this->getUser();
      if ($type == self::TYPE_ACL && $this->value > 0) {
        ACL::grantAccess($user, $item, $this->value);
      } 
      if (method_exists($item, 'onAppealAccept')) {
        $item->onAppealAccept($this);
      }
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
    }
  }

}