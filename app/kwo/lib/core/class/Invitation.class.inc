<?php

class Invitation extends ActiveRecord {

  const STATUS_BANNED = -10;
  const STATUS_REFUSED = -5;
  const STATUS_POSTPONED = -1;
  const STATUS_PENDING = 1;
  const STATUS_ACCEPTED = 10;

  public function accept() {
    $this->setStatus(self::STATUS_ACCEPTED);
    $item = $this->getItem();
    if (method_exists($item, 'onInvitationAccept')) {
      $item->onInvitationAccept($this);
    }
  }

  public function postpone() {
    $this->setStatus(self::STATUS_POSTPONED);
    $item = $this->getItem();
    if (method_exists($item, 'onInvitationPostpone')) {
      $item->onInvitationPostpone($this);
    }
  }

  public function refuse() {
    $status = $this->status === self::STATUS_REFUSED
            ? self::STATUS_REFUSED
            : self::STATUS_BANNED;
    $this->setStatus($status);
    $item = $this->getItem();
    if (method_exists($item, 'onInvitationRefuse')) {
      $item->onInvitationRefuse($this);
    }
  }

  public function send() {
    $item = $this->getItem();
    if (method_exists($item, 'onInvitationSend')) {
      $item->onInvitationSend($this);
    }
  }

  public function asInvitationSnippet() {
    return $this->getItem()->asInvitationSnippet($this);
  }

  public function getRecipient() { 
    return Item::getInstance((int) $this->recipient_model_id,
                             (int) $this->recipient_record_id);
  }

}