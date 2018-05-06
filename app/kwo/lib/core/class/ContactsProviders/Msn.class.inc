<?php

class msn {

  private $login_url = 'https://mid.live.com/si/login.aspx';
  private $contact_url = 'http://mpeople.live.com/default.aspx?pg={offset}';
  private $login = '';
  private $password = '';
  private $cookie_file = '/tmp/msn_contact_grabber.cookie';
  private $contacts = array();

  public function __construct($login, $mdp) {
    $this->login = $login;
    $this->password = $mdp;
    $this->cookie_file .= crc32($login);
    $this->connect();
  }
  public function __destruct() {
    if (is_file($this->cookie_file)) {
      unlink($this->cookie_file);
    }
  }
  public function getContacts() {
    if (count($this->contacts) > 0) {
      return $this->contacts;
    }
    $i = 0;
    while (true) {
      $ch = curl_init(str_replace('{offset}', $i, $this->contact_url));
      curl_setopt($ch, CURLOPT_HEADER, 1);
      curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
      curl_setopt($ch, CURLOPT_COOKIEJAR, $this->cookie_file);
      curl_setopt($ch, CURLOPT_COOKIEFILE, $this->cookie_file);
      curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
      curl_setopt($ch, CURLOPT_AUTOREFERER, true);
      preg_match_all('/href\="\/contactinfo\.aspx\?contactid\=[^"]+"\w*\>([^\<]+)\<\/a\>/', curl_exec($ch), $contacts);
      if (end($this->contacts) == end($contacts[1])) {
        break;
      }
      $this->contacts = array_merge($this->contacts, $contacts[1]);
      curl_close($ch);
      ++$i;
    }
    return $this->contacts;
  }
  private function connect() {
    $ch = curl_init($this->login_url);

    curl_setopt($ch, CURLOPT_HEADER, 1);
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_COOKIEJAR, $this->cookie_file);
    curl_setopt($ch, CURLOPT_COOKIEFILE, $this->cookie_file);
    curl_setopt($ch, CURLOPT_AUTOREFERER, true);
    curl_setopt($ch, CURLOPT_POST, true);
    curl_setopt($ch, CURLOPT_POSTFIELDS, array('LoginTextBox' => $this->login,
                                               'PasswordTextBox' => $this->password,
                                               'SavePasswordCheckBox' => 0,
                                               '__ET' => "",
                                               'PasswordSubmit' => 'PasswordSubmit'));
    $conn = curl_exec($ch);
    curl_close($ch);
  }
}