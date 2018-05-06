<?php

class Offer extends ActiveRecord {
  
  const FLAG_SKIP_INCOMPLETE = 1;
  
  public static $flags = array(self::FLAG_SKIP_INCOMPLETE => 'skip incomplete');

  const STATUS_DELIVERED = 100;
  const STATUS_PROCESSING = 90;
  const STATUS_SCHEDULED = 20;
  const STATUS_DRAFT = 0;
  const STATUS_CANCELED = -10;
  const STATUS_ERROR = -50;
  
  public static $statuses = array(self::STATUS_DELIVERED => 'finalisée',
                                  self::STATUS_PROCESSING => 'en cours d’envoi',
                                  self::STATUS_SCHEDULED => 'prête pour l’envoi', 
                                  self::STATUS_DRAFT => 'en cours de rédaction',
                                  self::STATUS_CANCELED => 'annulée',
                                  self::STATUS_ERROR => 'erreur');

  protected function onBeforeCommit(&$attrs) { 
    if ($attrs['status'] > self::STATUS_SCHEDULED && isset($attrs['name'])) {
      throw new Exception('statut invalide');
    }
    if (isset($attrs['user_statuses'])) {
//      $attrs['user_privileges'] = AH::asSet($attrs['user_privileges']);
      $attrs['user_statuses'] = AH::asSet($attrs['user_statuses']);
    }
    if ($attrs['notice_id'] > 0 && 
        isset($attrs['content']) && 
        stripos($attrs['content'], '((remove))') === false) {
      try {
        $snippet = Snippet::getInstance('offer.unsubscribe');
        $html = $snippet->getProperty('content', $attrs['locale']);
        $attrs['content'] .= $html;
      }
      catch (Exception $e) {
        S('log')->error($e, __METHOD__);
      }
    }
  }

  public function asEditHash() {
    $ht = array();
    if ($this->isItem()) {
      $ht['status_readonly'] = $this->status > self::STATUS_SCHEDULED;
    }
    else {
      $ht['locale'] = S('res')->getLocale();
      $ht['status'] = self::STATUS_DRAFT;
    }
    return $ht;
  }

  private function getUsers() {
    $filters = array();
    if ($this->locale > 0) {
      $filters['locale'] = $this->locale;
    }
    if (!empty($this->user_statuses)) {
      $filters['user_statuses'] = SH::asSet($this->user_statuses);
    }
    $fields = array('user_flags', 'user_notices', 
                    'user_profiles', 'user_privileges');
    foreach ($fields as $field) {
      $value = $this->getAttribute($field);
      if ($value < 1) continue ;
      $filters['|&|'.$field] = $value;
    }
    $opts = array('filters' => $filters);
    return User::collect($opts);
  }

  private $bindings = null;
  private $models = array();

  public function deliver($test_mode=true) {
    $tags = SH::extractTags($this->content.$this->subject, '((', '))');
    foreach ($tags as $tag) {
      if (strpos($tag, '.') < 1) {
        $this->bindings[$tag] = array('user', $tag);
        continue ;
      }
      list ($model, $attr) = explode('.', $tag);
      if (!in_array($model, $this->models)) {
        $this->models[] = $model;
      }
      $this->bindings[$tag] = array($model, $attr);
    }
    if ($test_mode === true) {
      $users = array();
      $recipients = SH::asSet($this->recipients);
      foreach ($recipients as $email) {
        try {
          $users[] = User::getInstance(array('email' => $email));
        }
        catch (Exception $e) {
          S('log')->error($e, __METHOD__);
        }
      }
    }
    else {
      $this->setStatus(self::STATUS_PROCESSING);
      $users = $this->getUsers();
    }
    foreach ($users as $user) {
      try {
        $this->send($user);
      }
      catch (Exception $e) { 
        //error_log($e->getLine().$e->getFile());
        S('log')->error($e, __METHOD__);
      }
    }
    if ($test_mode === false) {
      $this->setStatus(self::STATUS_DELIVERED);
      $this->commit(array('delivered_at' => DH::now()));
    }
  }

  private function send($user) {
    $items = array('user' => $user);
    foreach ($this->models as $model) {
      $items[$model] = $user->getOne($model);
    }
    $bindings = array();
    foreach ($this->bindings as $tag => $ht) {
      $model = $ht[0];
      $attr = $ht[1];
      if ($model === 'user' && $attr === 'item') {
        $value = $user->getKey();
      }
      elseif ($model === 'user' && $attr === 'remove') {
        $user_id = $user->id;
        $value = H::url('/notice.remove', 
                        array('uid' => $user->id, 
                              'nid' => $this->notice_id, 
                              'h' => SH::crypt($this->notice_id . $user->id)));
      }
      elseif ($attr[0] === '*') {
        $method = substr($attr, 1);
        $value = call_user_func(array($items[$model], $method));
      }
      else { 
        $value = $items[$model]->getAttribute($attr);
      }
      $key = '(('.$tag.'))';
      if (empty($value) && $this->hasFlag(self::FLAG_SKIP_INCOMPLETE)) {
        return ;
      }
      $bindings[$key] = $value;
    }
    $subject = str_replace(array_keys($bindings), array_values($bindings),
                           $this->subject);
    $content = str_replace(array_keys($bindings), array_values($bindings),
                           $this->content);

    $mail = new Mail();
    if (!empty($this->from_email)) {
      $mail->setFrom($this->from_email, $this->from_name);
    }
    $mail->addHeader('Precedence', 'bulk');
    $mail->addRecipient($user);
    if (!empty($this->reply_email)) {
      $mail->setReplyTo($this->reply_email);
    }
    $mail->setSubject($subject);
    $mail->setBodyHtml($content); 
    //error_log($content);
    $mail->send();
    
  }

}