<?php

class Base extends ActiveRecord
{
  const FLAG_PUBLIC = 1;
  const FLAG_PARTNER = 2;
  const FLAG_TEST = 4;
  const FLAG_USER = 8;
  const FLAG_CUSTOMER = 16;

  const TABLE_EMAIL = 'core_base_email';

  public static $flags = array(self::FLAG_PUBLIC => 'publique',
                               self::FLAG_PARTNER => 'partenaire',
                               self::FLAG_TEST => 'test',
                               self::FLAG_USER => 'utilisateur',
                               self::FLAG_CUSTOMER => 'client');

  public function getDetails() {
    $count = $this->dbo->fetchValue('SELECT COUNT(email_id)'
                                    .' FROM '.self::TABLE_EMAIL
                                    .' WHERE base_id='.$this->getId()
                                    .  ' AND status=1');
    return array('nbre contacts' => $count);
  }

  public function asFinderHash($h) {
    if ($this->hasFlag(self::FLAG_TEST)) {
      $h['code'] .= ' *';
    }
    return $h;
  }


  public function addEmail($email, $force=false) {
    $locale = 0;
    $email_id = Email::asId($email);
    $row = array('base_id' => $this->getId(),
                 'email_id' => $email_id);
    $table = $this->dbo->getTable(self::TABLE_EMAIL);
    if (!$table->hasRow($row)) {
      $row['status'] = 1;
      $table->insert($row);
      return ;
    }
    if ($force === false) return ;
    $this->dbo->exec('UPDATE '.self::TABLE_EMAIL
                     .' SET status=1'
                     .' WHERE email_id='.$email_id
                     .  ' AND base_id='.$this->getId()
                     .' LIMIT 1');
  }

  public function hasEmail($email) {
    $email_id = Email::asId($email);
    return $this->dbo->fetchValue('SELECT COUNT(*)'
                                  .' FROM '.self::TABLE_EMAIL
                                  .' WHERE email_id='.$email_id
                                  .  ' AND base_id='.$this->getId()
                                  .  ' AND status=1'
                                  .' LIMIT 1') > 0;
  }

  public function removeEmail($email) {
    $email_id = Email::asId($email);
    $this->dbo->exec('UPDATE '.self::TABLE_EMAIL
                     .' SET status=0'
                     .' WHERE email_id='.$email_id
                     .  ' AND base_id='.$this->getId()
                     .' LIMIT 1');
  }

  public static function addUser($user, $flag=self::FLAG_USER) {
    try {
      if ($user instanceof User) {
        $locale = $user->locale;
        $email = $user->email;
      }
      else {
        $locale = S('res')->getLocale();
        $email = $user;
      }
      $filters = array('|&|flags' => $flag,
                       'locale' => $locale);
      $opts = array('filters' => $filters);
      $bases = Base::collect($opts);
      foreach ($bases as $base) {
        $base->addEmail($email);
      }
    }
    catch (Exception $e) { S('log')->error($e); }
  }

  public static function addCustomer($user) {
    Base::addUser($user, self::FLAG_CUSTOMER);
  }

  public static function onEmailChange($from, $to) {
    $from_id = Email::asId($from);
    $to_id = Email::asId($to);
    $bases = Base::collect('base');
    $dbo = S('dbo');
    foreach ($bases as $base) {
      if (!$base->hasEmail($from_id)) continue ;
      $base->removeEmail($from_id);
      if (!$base->hasEmail($to_id)) {
        $base->addEmail($to_id);
      }
    }
  }

}