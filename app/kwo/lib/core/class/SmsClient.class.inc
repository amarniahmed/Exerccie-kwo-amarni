<?php

/*
 * DOC : http://api.smsbox.fr/readme.html
 * TODO : sms multimedia
 */

class SmsClient {

  private $from = null;
  private $methode = 'Expert';
  private $login = null;
  private $pass = null;
  private $date = null;
  private $time = null;
  private $rcpt = array();
  private $retour = null;
  private $references = null;

  private static $url = 'https://api.smsbox.fr/api.php';
  private static $errors = array('OK' => 'Message envoyé avec succès',
                                 'ERROR 01' => 'Paramètres manquants',
                                 'ERROR 02' => 'Identifiant ou mot de passe incorrect',
                                 'ERROR 03' => 'Crédit insuffisant',
                                 'ERROR 04' => 'Numéro invalide',
                                 'ERROR 05' => 'Erreur d\'éxécution SMSBOX');

  public function __construct($opts) {
    $this->log = S('log');
    $this->login = $opts['login'] ?: P('sms.login');
    $this->pass = $opts['password'] ?: P('sms.password');
    $this->from = $opts['from'] ?: (P('sms.from') ?: 'kernix');
    $this->setSendDate(time());
  }
  
  public function setMethode($methode) {
    $this->methode = $methode;
    return $this;
  }
  
  public function setFrom($from) {
    $this->from = $from;
    return $this;
  }
  
  public function setSendDate($ts) {
    $this->date = date('d/m/Y', $ts);
    $this->time = date('H:i', $ts);
    return $this;
  }
  
  public function addRecipients($numbers) {
    foreach ($numbers as $number) {
      $this->addRecipient($number);
    }
  }

  public function addRecipient($number) {
    $number = preg_replace('/\D+/', '', $number);
    if (empty($number)) {
      return false;
    }
    $this->rcpt[] = $number;
    return $this;
  }

  public function send($msg) {
    if (empty($msg)) {
      $this->log->error('empty message', __METHOD__);
      throw new Exception('empty msg');
    }
    
    $number = implode(',', $this->rcpt);
    $msg = mb_convert_encoding($msg, 'ISO-8859-1', 'UTF-8');
    $params = array('login' => $this->login,
                    'pass' => $this->pass,
                    'msg' => $msg,
                    'dest' => $number,
                    'id' => 1,
                    'date' => $this->date,
                    'heure' => $this->time,
                    'origine' => $this->from,
                    'mode' => $this->methode);
    $url = self::$url . '?' . http_build_query($params);
    $this->retour = file_get_contents($url);
    $this->rcpt = array();
    if ($this->getErrorCode() === 'OK') {
      return true;
    }
    $this->log->error('sending failure ['.$this->getError().']', __METHOD__);
    return false;
  }
  public function getReferences() {
    if ($this->references === null) {
      if ($this->getErrorCode() === 'OK') {
        $this->references = explode(',', substr($this->retour, 3));
      } else {
        $this->references = null;
      }
    }
    return $this->references;
  }

  public function getReference() {
    $this->getReferences();
    return $this->references[0];
  }

  public function getErrorCode() {
    if (substr($this->retour, 0, 2) === 'OK') {
      return 'OK';
    }
    return $this->retour;
  }

  public function getError() {
    return self::$errors[$this->getErrorCode()].' ('.$this->getErrorCode().')';
  }

  public static function getInstance($opts=null) {
    $opts = (array) $opts;
    return new SmsClient($opts);
  }
}