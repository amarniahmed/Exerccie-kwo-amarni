<?php

class Contact extends ActiveRecord 
{
  protected function onBeforeCommit(&$hash) {
    if ($this->isItem() && !isset($hash['email'])) {
      throw new ActiveRecordException('email required');
    }
    if (isset($hash['email'])) {
      $hash['email_id'] = Email::asId($hash['email']);
      unset($hash['email']);
    }
  }
  
  public function asEditHash() {
    //    $hash = parent::asEditHash();
    if ($this->isItem()) {
      $hash['email'] = $this->dbo->fetchValue('SELECT email'
                                              .' FROM push_email'
                                              .' WHERE id='.intval($this->email_id));
    }
    return $hash;
  }

  public function unsub($campaign_id) {
    Act::add(array('email_id' => $this->email_id,
                   'base_id' => $this->base_id,
                   'campaign_id' => $campaign_id,
                   'type' => Act::TYPE_REMOVE,
                   'value' => 0));
    $this->commit(array('is_subscribed' => 0));
  }

}