<?php

Platform::import('SmtpLog');

class Scenario {
  private $base      = array(1,2,3);
  private $status    = array(0,1,2,3);
  private $domain    = array("kernix.com","wanadoo.fr","gmail.com","caramail.fr","hotmail.com","yahoo.com","mymail.com","skynet.be","mailbox.net","voila.fr","lycos.uk","laposte.net");
  private $civility  = array('Mlle','Mr','Mme');
  private $gender    = array('H', 'F');
  private $street    = array("rue", "boulevard","avenue");
  
  public function __construct() {
    $this->dbo        = Platform::getSingleton('dbo');
    $this->cnf       = Platform::getSingleton('cnf');
    
    $this->dbo->exec("TRUNCATE TABLE `push_event_out`");
    $this->dbo->exec("TRUNCATE TABLE `push_event_in`");
    $this->dbo->exec("TRUNCATE TABLE `push_contact`");
  }
  
  public function add_user() {
    $l_md5 = md5(mktime().microtime());
    $l_base = $this->base[array_pop(array_rand($this->base,2))];
    $l_mail = substr($l_md5,0,mt_rand(5,15))."@".$this->domain[array_pop(array_rand($this->domain,2))];
    
    $f = array();
    $f["base_id"] = $l_base;
    $f["contact_key"] = substr($l_md5,1,10);
    $f["status"] = $this->status[array_pop(array_rand($this->status,2))];
    $f["email"] = $l_mail;
    $f["login"] = substr($l_md5,0,mt_rand(5,15));
    $f["password"] = substr($l_md5,0,mt_rand(5,15));
    $f["gender"] = $this->gender[array_pop(array_rand($this->gender,2))];
    $f["civility"] = $this->civility[array_pop(array_rand($this->civility,2))];
    $f["last_name"] = substr($l_md5,0,mt_rand(5,15));
    $f["first_name"] = substr($l_md5,0,mt_rand(5,15));
    $f["home_phone"] = "0".mt_rand(111111111,999999999);
    $f["work_phone"] = "0".mt_rand(111111111,999999999);
    $f["mobile_phone"] = "06".mt_rand(11111111,99999999);
    $f["fax"] = "0".mt_rand(111111111,999999999);
    $f["address_1"] = mt_rand(1,99)." ".$this->street[array_pop(array_rand($this->street,2))]." ".substr($l_md5,0,mt_rand(5,mt_rand(5,15)));
    $f["address_2"] = mt_rand(1,99)." ".$this->street[array_pop(array_rand($this->street,2))]." ".substr($l_md5,0,mt_rand(5,mt_rand(5,15)));
    $f["address_3"] = mt_rand(1,99)." ".$this->street[array_pop(array_rand($this->street,2))]." ".substr($l_md5,0,mt_rand(5,mt_rand(5,15)));
    $f["address_4"] = mt_rand(1,99)." ".$this->street[array_pop(array_rand($this->street,2))]." ".substr($l_md5,0,mt_rand(5,mt_rand(5,15)));
    $f["zip_code"] = mt_rand(11111,99999);
    $f["city"] = substr($l_md5,0,mt_rand(5,mt_rand(5,15)));
    $f["state"] = substr($l_md5,0,mt_rand(5,mt_rand(5,15)));
    $f["country_code"] = substr($l_md5,0,2);
    $f["corporate_name_1"] = substr($l_md5,0,mt_rand(5,mt_rand(5,15)));
    $f["corporate_name_2"] = substr($l_md5,0,mt_rand(5,mt_rand(5,15)));
    $f["corporate_code_1"] = substr($l_md5,0,mt_rand(5,mt_rand(5,15)));
    $f["corporate_code_2"] = substr($l_md5,0,mt_rand(5,mt_rand(5,15)));
    $f["tax_authority_reference"] = substr($l_md5,0,mt_rand(5,mt_rand(5,32)));
    $f["staff_id"] = substr($l_md5,0,mt_rand(5,mt_rand(5,32)));
    $f["business_id"] = substr($l_md5,0,mt_rand(5,mt_rand(5,64)));
    $f["business_code"] = substr($l_md5,0,mt_rand(5,mt_rand(5,64)));
    $f["role_id"] = substr($l_md5,0,mt_rand(5,mt_rand(5,64)));
    for ($i=0,$tmp=substr($l_md5,0,mt_rand(5,mt_rand(5,25)));$i<10;$i++) $tmp .= " ".substr($l_md5,0,mt_rand(5,mt_rand(5,25)));
    $f["custom_str_1"] = $tmp;
    for ($i=0,$tmp=substr($l_md5,0,mt_rand(5,mt_rand(5,25)));$i<10;$i++) $tmp .= " ".substr($l_md5,0,mt_rand(5,mt_rand(5,25)));
    $f["custom_str_2"] = $tmp;
    for ($i=0,$tmp=substr($l_md5,0,mt_rand(5,mt_rand(5,25)));$i<10;$i++) $tmp .= " ".substr($l_md5,0,mt_rand(5,mt_rand(5,25)));
    $f["custom_str_3"] = $tmp;
    for ($i=0,$tmp=substr($l_md5,0,mt_rand(5,mt_rand(5,25)));$i<10;$i++) $tmp .= " ".substr($l_md5,0,mt_rand(5,mt_rand(5,25)));
    $f["custom_str_4"] = $tmp;
    $f["origin"] = substr($l_md5,0,mt_rand(5,mt_rand(5,64)));
    $f["birth_date"] = date("Y-m-d", mt_rand(1000000,759957190));
    $f["optin"] = substr($l_md5,mt_rand(0,25),2);
    $f["upd_date"] = DH::now();
    $f["ins_date"] =  DH::now();
    $l_id = $this->dbo->insert('push_contact', $f, true);
    $this->debug("contact [$l_id][$l_base][$l_mail]");
    return array($l_id,$l_mail,$l_base);
  }
  
  public function add_event_in ($l_id, $l_base, $l_campaign, $l_sending, $l_type) {
    $f = array();
    $f["contact_id"] = $l_id;
    $f["base_id"] = $l_base;
    $f["campaign_id"] = $l_campaign;
    $f["sending_id"] = $l_sending;
    $l_type_arr = $this->cnf->getValue("event_type_arr");
    switch ($l_type) {
    case 0 :
      $f["type"] = $l_type;
      $f["interest_id"] = mt_rand(0,10);
    case 1 :
      $f["type"] = $l_type;
      $f["data"] = "link".mt_rand(1,10);
      break;
    case 2 :
      $f["type"] = $l_type;
      $f["interest_id"] = mt_rand(0,10);
      break;
    case 4 :
      $f["type"] = $l_type;
      $f["interest_id"] = mt_rand(0,10);
      break;
    case 5 :
      $f["type"] = $l_type;
      $f["data"] = $this->domain[array_pop(array_rand($this->domain,2))];
      $f["code"] = SmtpLog::$soft_code_arr[array_pop(array_rand(SmtpLog::$soft_code_arr,2))];
      break;
    case 6 :
      $f["type"] = $l_type;
      $f["data"] = $this->domain[array_pop(array_rand($this->domain,2))];
      $f["code"] = SmtpLog::$hard_code_arr[array_pop(array_rand(SmtpLog::$hard_code_arr,2))];
      break;
    default:
      $f["type"] = $l_type;
      $f["data"] = "";
      break;
    }
    $f["ins_date"] = DH::now();
    $this->dbo->insert('push_event_in', $f, true);
    $this->debug("[".$l_type_arr[$l_type]."] ");
  }
  
  public function add_event_out ($l_id, $l_mail, $l_base, $l_status, $l_campaign, $l_sending) {
    $f = array();
    $f["contact_id"] = $l_id;
    $f["base_id"] = $l_base;
    $f["campaign_id"] = $l_campaign;
    $f["sending_id"] = $l_sending;
    $f["email"] = $l_mail;
    $f["status"] = $l_status;
    $f["ins_date"] = DH::now();
    $this->dbo->insert('push_event_out', $f, true);

    $this->debug("[out] ");
  }
  
  function debug ($str) {
    print($str);
  }
}
