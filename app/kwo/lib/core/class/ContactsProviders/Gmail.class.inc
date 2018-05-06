<?php

class GmailContacts {
  
  private $params = array('accountType' => 'GOOGLE',
                          'service' => 'cp');
  
  public function __construct($email, $pass) {
    $this->params['Email'] = $email;
    $this->params['Passwd'] = $pass;
    $h = curl_init('https://www.google.com/accounts/ClientLogin');
    curl_setopt($h, CURLOPT_POST, true);
    curl_setopt($h, CURLOPT_POSTFIELDS, $this->params);
    curl_setopt($h, CURLOPT_RETURNTRANSFER, true);
    $response = curl_exec($h);
    if (preg_match('/^Error/', $response)) {
      throw new Exception('Bad Google authentication');
    }
    list (,$this->sid,, $this->lsid,, $this->auth) = preg_split('/\n|=/', $response);
    curl_close($h);
  }

  public function getContacts(array $opts = array('start' => 1, 'max' => 9999999)) {
    $contacts = array();
    $h = curl_init('http://www.google.com/m8/feeds/contacts/default/full?'.
                   'start-index='.(int)$opts['start'].'&max-results='.(int)$opts['max']);
    curl_setopt($h, CURLOPT_HTTPGET, true);
    curl_setopt($h, CURLOPT_HTTPHEADER, array('Authorization: GoogleLogin auth='.$this->auth));
    curl_setopt($h, CURLOPT_RETURNTRANSFER, true);
    $doc = new DOMDocument();
    $doc->preserveWhiteSpace = false;
    if ($doc->loadXML(curl_exec($h)) === false) {
      throw new Exception('invalid xml in ['.__METHOD__.']', __METHOD__);
    }
    foreach ($doc->documentElement->childNodes as $node) {
      if ($node->tagName == 'entry') {
        $name = $email = '';
        foreach ($node->childNodes as $entry) {
          if ($entry->tagName == 'title') $name = utf8_decode($entry->nodeValue);
          if ($entry->tagName == 'gd:email') $email = $entry->getAttribute('address');
        }
        if (empty($name)) $name = $email;
        if (strpos($email, '@') === false) continue ;
        $contacts[] = array('name' => $name, 'email' => $email);
      }
      elseif (substr($node->tagName, 0, 10) == 'openSearch') {
        list(, $prop) = explode(':', $node->tagName);
        $this->$prop = $node->nodeValue;
      }
    }
    return $contacts;
  }
}