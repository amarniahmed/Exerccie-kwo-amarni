<?php

class ImportContacts {

  public static $providers = array(1 => 'Gmail', 
                                   2 => 'MSN');

  public static function factory ($user, $pass, $type) {
    if (self::$providers[$type] === null) {
      throw new Exception('invalid type, use: '.
                          implode (', ', self::$providers));
    }
    $class = self::$providers[$type].'Decorator';
    return new $class($user, $pass);
  }
}

class GmailDecorator {
  private $_instance;

  public function __construct($user, $pass) {
    require 'ContactsProviders/Gmail.class.inc';
    $this->_instance = new GmailContacts($user, $pass);
  }	
  
  public function getContacts() {
    if (!is_object ($this->_instance))
      throw new Exception('Aucune instance');
    return $this->_instance->getContacts();
  }
}

class MSNDecorator {
  private $_instance;
  private $_user;
  private $_pass;

  public function __construct($user, $pass) {
    require 'ContactsProviders/Msn.class.inc';
    $this->_instance = new msn($user, $pass);
  }

  public function getContacts() {
    $i = 0;
    $result = array ();
    foreach (array_unique($this->_instance->getContacts()) as $contact) {
      $result[$i]['name'] = $contact;
      $result[$i]['email'] = $contact;
      $i++;
    }
    return $result;
  }
}

class YahooDecorator {
  private $_instance;
  private $_user;
  private $_pass;
  
  public function __construct($user, $pass) {
    require 'ContactsProviders/Yahoo.class.inc';
    $this->_instance = new yahooGrabber($user, $pass);
    $this->_user = $user;
    $this->_pass = $pass;
  }

  public function getContacts() {
    $result = array ();
    if (!is_object ($this->_instance)) {
      throw new Exception('invalid yahoo lib');
    }
    $contacts = (array) @$this->_instance->grabYahoo ();
    $i = 0;
    foreach ($contacts as $name => $email) {
      $result[$i]['name'] = $name;
      $result[$i]['email'] = $email;
      $i++;
    }
    return $result;
  }
}

