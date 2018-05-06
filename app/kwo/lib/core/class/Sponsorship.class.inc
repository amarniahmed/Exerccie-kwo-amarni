<?php

class Sponsorship extends ActiveRecord {

  public function getProfit() {
    if ($this->godson_id < 1) return 0;
    return $this->dbo->fetchValue('SELECT SUM(amount)'
                                  .' FROM '.T('credit')
                                  .' WHERE user_id='.$this->godson_id
                                  .  ' AND sponsor_id='.$this->user_id);
  }

  public function markAsAcceptedBy($user) {
    if ($this->godson_id > 0) return ;
    try {
      $this->setAttribute('godson_id', $user->getId());
      $user->setFlag(User::FLAG_GODSON);
      if ($this->user_id > 0) {
        User::getInstance($this->user_id)->setFlag(User::FLAG_SPONSOR);
      }
      S('app')->hook('onSponsorshipAccept', array('sponsorship' => $this));
    }
    catch (Exception $e) {
      S('log')->error($e, __METHOD__);
    }
  }

  public function send() {
    $user = $this->getUser();
    if (!$user->isOnline()) return false;
    if ($this->godson_id >= 1) return false;
    $content = empty($this->content) ? '' : '<br/><em>'.nl2br($this->content).'</em>';
    $link = '/?knxsid='.$this->getId();
    $name = empty($this->name) ? '' : ' '.$this->name;

    $first_name = $user->name;
    if ($user->hasProfile()) {
      $profile = $user->getProfile();
      if (!empty($profile->first_name)) {
        $first_name = $profile->first_name;
      }
    }
    $from_name = $first_name.' - '.P('mail.from.name');
    $reply_name = $first_name;
    $opts = array('from' => array('name' => $from_name,
                                  'email' => $user->email),
                  'reply-to' => array('name' => $reply_name,
                                      'email' => $user->email));
    Letter::send('sponsorship',
                 $this->email_id,
                 array('name' => $name,
                       'content' => $content,
                       'link' => $link),
                 $opts);
    return true;
  }

  public static function deliver($user, $email, $opts=array()) {
    try {
      if (!Email::isValid($email, true)) return false;
      $email_id = Email::asId($email);
      $attributes = array('user_id' => $user->getId(),
                          'email_id' => $email_id);
      if (Item::exists('sponsorship', $attributes)) {
        return false;
      }
      $attributes['content'] = SH::clean($opts['content'], array('strict' => true));
      $attributes['name'] = SH::clean($opts['name'], array('strict' => true));
      $sponsorship = Sponsorship::getInstance();
      $sponsorship->commit($attributes);
      $sponsorship->send();
      return true;
    }
    catch (Exception $e ) {
      S('log')->error($e, __METHOD__);
      return false;
    }
  }

}