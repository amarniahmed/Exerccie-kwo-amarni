<?php

class Error implements Countable
{
  const CODE_DEFAULT = 1;
  const CODE_EXCEPTION = 2;
  const CODE_AUTH_FAILURE = 401;

  const CODE_DISPATCH = 1024;
  const CODE_INVALID_URL = 1025;
  const CODE_INVALID_CONTROLLER = 1026;
  const CODE_INVALID_EXTENSION = 1027;

  private $code = 0;
  private $messages = array();
  private $errors = array();
  private $data = array();
  
  public function __construct() { }

  public function add($message='error', $code=self::CODE_DEFAULT) {
    if ($message instanceof Exception) {
      $code = $message instanceof AuthException
            ? self::CODE_AUTH_FAILURE
            : $message->getCode();
      $code = (int) $code;
      $code = $code === 0 ? self::CODE_EXCEPTION : $code;
      $message = $message->getMessage();
    }
    if (is_array($message)) {
      /* key, value, label */
      $this->errors[] = $message;
    }
    else {
      $this->messages[] = $message;
    }
    $this->setCode($code);
  }
  
  public function setCode($code) {
    $code = (int) $code;
    if ($code === 0) {
      S('log')->error('invalid code value (0) : '.$message, __METHOD__);
    }
    $this->code = $code === 0 ? self::CODE_DEFAULT : $code;
  }

  public function getCode() {
    return $this->code;
  }

  public function getData() { 
    return $this->data; 
  }

  public function setData($key, $value) { 
    $this->data = (array) $this->data;
    $this->data[$key] = $value; 
  }

  public function getMessage() {
    return join("\n", $this->messages);
  }

  public function getMessages() { 
    return $this->messages; 
  }

  public function count() { 
    return count($this->messages); 
  }

  public function debug() {
    S('log')->debug($this->messages, 'code='.$this->code);
    if (count($this->data) > 0) {
      S('log')->debug($this->data, 'data'); 
    }
  }

  public static function getInstance() {
    if (Platform::hasSingleton('err')) return ;
    Platform::addSingleton('err', new Error());
  }

}
