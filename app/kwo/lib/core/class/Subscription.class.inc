<?php

class Subscription extends ActiveRecord {

  public static function save($item, $recipient) {
    $row = array('model_id' => $item->getModelId(),
                 'record_id' => $item->getId());
    if ($recipient instanceof User) {
      $email_id = Email::asId($recipient->email);
    }
    elseif (is_numeric($recipient)) {
      $email_id = (int) $recipient;
    }
    else {
      $email_id = Email::asId($recipient);
    }
    $row['email_id'] = $email_id;
    if (Item::exists('subscription', $row)) {
      return null;
    }
    $subscription = Subscription::getInstance();
    $subscription->commit($row);
    if (method_exists($item, 'onSubscription')) {
      $item->onSubscription($favorite);
    }
    $item->track(Interaction::TYPE_SUBSCRIBE, 
                 array('value' => $subscription->getId(),
                       'status' => 1));
    return $subscription;
  }

  public static function onEmailChange($from, $to) {
    $from_id = Email::asId($from);
    $to_id = Email::asId($to);
    $dbo = S('dbo');
    $table = $dbo->getTable(T('subscription'));
    $rs = $dbo->query('SELECT model_id, record_id'
                      .' FROM '.T('subscription')
                      .' WHERE email_id='.$from_id);
    foreach ($rs as $row) {
      $row['email_id'] = $to_id;
      if ($table->hasRow($row)) continue ;
      $dbo->exec('UPDATE '.T('subscription')
                 . ' SET email_id='.$to_id
                 .' WHERE model_id='.$row['model_id']
                 .  ' AND record_id='.$row['record_id']
                 .  ' AND email_id='.$from_id
                 .' LIMIT 1');
    }
  }

}