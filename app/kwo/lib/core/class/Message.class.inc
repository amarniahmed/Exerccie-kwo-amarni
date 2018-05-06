<?php

/***
type : admin, newsletter, event, request, announce
***/

class Message extends ActiveRecord {

  const INBOX_LIMIT = 10;

  const STATUS_SPAM = -10;
  const STATUS_DELETED = 0;
  const STATUS_SENT = 1;
  const STATUS_READ = 2;

  const TABLE_RECIPIENTS = 'core_message_user';

  const FLAG_DIRECT = 1;
  const FLAG_ADMIN = 2;
  const FLAG_CLOSED = 4;
  const FLAG_RESPONSE_REQUIRED = 8;

  public static $flags = array(self::FLAG_DIRECT => 'direct',
                               self::FLAG_ADMIN => 'admin',
                               self::FLAG_CLOSED => 'fermé',
                               self::FLAG_RESPONSE_REQUIRED => 'obligatoire');

  public function onRelationCreate($item) {
    if ($item->isA('comment')) {
      $comment = $item;
      $user = $comment->getUser();
      $this->commit(array('replier_id' => $user->getId()));
      $this->setMeta(Meta::FIELD_UPDATED_AT, DH::now());
      $opts = array('filters' => array('message_id' => $this->getId()));
      $record = array('status' => 1);
      $this->dbo->update(self::TABLE_RECIPIENTS, $record, $opts);
      $rs = $this->dbo->query('SELECT u.locale, u.email'
                              .' FROM '.self::TABLE_RECIPIENTS.' AS mu'
                              .' JOIN '.T('user').' AS u ON (mu.user_id=u.id)'
                              .' WHERE mu.message_id='.$this->getId()
                              .  ' AND mu.user_id<>'.$user->getId());
      $bindings = array('title' => $this->title,
                        'content' => nl2br($comment->content),
                        'name' => $user->name,
                        'link' => $this->asUrl());
      $opts = array();
      foreach ($rs as $hash) {
        $opts['locale'] = $hash['locale'];
        Letter::send('inbox.message.update', $hash['email'], $bindings, $opts);
      }
    }
  }

  public function recipients() {
    return $this->dbo->asHash('SELECT u.id, u.name'
                              .' FROM '.self::TABLE_RECIPIENTS.' AS m'
                              .' JOIN '.T('user').' AS u ON (m.user_id=u.id)'
                              .' WHERE m.message_id='.$this->getId()
                              .' ORDER BY u.name');
  }

  public function hasRecipient($user) {
     return $this->dbo->fetchValue('SELECT COUNT(*)'
                                   .' FROM '.self::TABLE_RECIPIENTS
                                   .' WHERE message_id='.$this->getId()
                                   .  ' AND user_id='.$user->getId()) >= 1;
  }

  public function markAsRead($user) {
    $ret = $this->dbo->fetchValue('SELECT status'
                                  .' FROM '.self::TABLE_RECIPIENTS
                                  .' WHERE message_id='.$this->getId()
                                  .  ' AND user_id='.$user->getId());
    if ($ret == 1) {
      $opts = array('filters' => array('message_id' => $this->getId(),
                                       'user_id' => $user->getId()),
                    'limit' => 1);
      $record = array('status' => 2);
      $this->dbo->update(self::TABLE_RECIPIENTS, $record, $opts);
    }
  }

  public function delete($user) {
    $opts = array('filters' => array('message_id' => $this->getId(),
                                     'user_id' => $user->getId()),
                  'limit' => 1);
    $record = array('status' => 0);
    $this->dbo->update(self::TABLE_RECIPIENTS, $record, $opts);
  }

  /* $recipients : peut être collection de User */
  public static function send($user, $recipients, $title, $content, $opts=array()) {
    try {
      $content = nl2br($content);
      $message = Message::getInstance();
      $attributes = array('title' => SH::clean($title),
                          'content' => SH::filter($content));
      $message->commit($attributes);
      if ($opts['direct'] === true) {
        $message->setFlag(Message::FLAG_DIRECT);
      }
      $message->setUser($user);
      $bindings = array('title' => $message->title,
                        'content' => $message->content,
                        'name' => $user->name,
                        'link' => $message->asUrl());
      $row = array('message_id' => $message->getId(),
                   'status'  => self::STATUS_READ,
                   'user_id' => $user->getId());
      S('dbo')->insert(self::TABLE_RECIPIENTS, $row);
      $recipients = $recipients instanceof ActiveRecord
                  ? array($recipients)
                  : $recipients;
      foreach ($recipients as $recipient) {
        $row['user_id'] = $recipient->getId();
        $row['status'] = self::STATUS_SENT;
        if ($recipient->status < 1) continue ;
        S('dbo')->insert(self::TABLE_RECIPIENTS, $row, array('modifiers' => 'ignore'));
        if ($opts['silent'] != true && ($recipient->hasNotice(Notice::INBOX))) {
          Letter::send('inbox.message.new', $recipient, $bindings);
        }
      }
      return $message;
    }
    catch (Exception $e) { S('log')->error($e, __METHOD__); }
  }

}