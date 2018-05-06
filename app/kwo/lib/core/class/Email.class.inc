<?php

class Email extends ActiveRecord
{
  const FLAG_OPTIN = 1;
  const FLAG_IMPORT = 2;
  const FLAG_ALTERED = 4;

  public function asFinderHash($h) {
    if ($this->status <= -1) $color = 'red';
    elseif ($this->status >= 0 && $this->status <= 3) $color = 'orange';
    elseif ($this->status > 3 && $this->status <= 6) $color = 'yellow';
    elseif ($this->status > 6 && $this->status <= 7) $color = 'blue';
    else $color = 'green';
    $h['_color'] = $color;
    return $h;
  }

  public function getConversionWorth($opts) {
    return 0.1;
  }

  public function getName($locale=null) { return $this->email; }
  
  public function onAfterDestroy() {
    $dbo = $this->dbo;
    $dbo->exec('DELETE FROM '.Base::TABLE_EMAIL
               .    ' WHERE email_id='.$this->id);
    $dbo->exec('DELETE FROM '.T('sponsorship')
               .    ' WHERE email_id='.$this->id);
    $rs = $this->dbo->query('SELECT user_id'
                            .' FROM '.User::TABLE_INFORMATION
                            .' WHERE recipient_id='.$this->id);
    foreach ($rs as $row) {
      $user = User::getInstance($row['user_id']);
      $user->setInformation('recipient_id', Email::asId($user->email));
    }
  }

  /* vérifier les règles sur les noms de domaine */
  /* ex. gmail : obligation d'un chiffre */

  public static $domains = array('hotmail.com', 'yahoo.fr', 'gmail.com', 'hotmail.fr', 'hotmail.be',
                                 'free.fr', 'yahoo.com', 'outlook.com',
                                 'caramail.com', 'netcourrier.com', 'bluewin.ch', 'numericable.fr', 'sfr.fr',
                                 'neuf.fr', 'laposte.net', 'laposte.fr', 'tele2.fr', 'libertysurf.fr',
                                 'msn.com', 'live.fr', 'wanadoo.fr', 'orange.fr', 'aliceadsl.fr', 'skynet.be',
                                 'cegetel.net', 'aol.com', 'club-internet.fr', 'tiscali.fr', 'voila.fr',
                                 'videotron.ca', 'noos.fr', 'me.com');

  public static function isValid(&$address, $opts=null) {
    $opts = $opts === true ? array('mx' => true, 'smtp' => false) : (array) $opts;
    if (strlen($address) <= 7) return false;
    if (stripos($address, 'caramail.com') >= 1) return false;
    if (filter_var($address, FILTER_VALIDATE_EMAIL) === false) {
      return false;
    }
    if (empty($opts)) return true;
    list (, $domain) = explode('@', $address);
    if (in_array($domain, array('yopmail.com', 'jetable.org', 'trillianpro.com', 'spamgourmet.com'))) {
      return false;
    }
    $domain = strtolower($domain);
    if ($opts['mx'] === true &&
        $opts['smtp'] !== true &&
        in_array($domain, self::$domains)) return true;
    $hosts = $weights = array();
    if (getmxrr($domain, $hosts, $weights) === false) return false;
    if ($opts['smtp'] === true) {
      $hosts = array_combine($hosts, $weights);
      asort($hosts);
      $host = key($hosts);
      return SmtpClient::validate($address, $host);
    }
    return true;
  }

  public static function asId($email) {
    if (is_numeric($email)) return (int) $email;
    if ($email instanceof Email) return (int) $email->getId();
    $email = trim($email);
    $id = S('dbo')->fetchValue('SELECT id'
                               .' FROM '.T('email')
                               .' WHERE email=:email:',
                               array(':email:' => $email));
    if ($id >= 1) return $id;
    $email = Email::getInstance()->commit(array('email' => $email));
    return $email->getId();
  }


}