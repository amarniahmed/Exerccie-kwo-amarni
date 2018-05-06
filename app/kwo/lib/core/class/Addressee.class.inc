<?php

class Addressee extends ActiveRecord {

  const FLAG_ELEVATOR = 1;

  public static $flags = array(self::FLAG_ELEVATOR => 'ascenseur');

  public function asAddress($locale=null) {
    $ret = '';
    $ret .= ucfirst($this->first_name).' '.ucfirst($this->last_name)."\n";
    $ret .= $this->address."\n";
    if (!empty($this->address_extra)) {
      $ret .= $this->address_extra."\n";
    }
    if (!empty($this->region)) {
      $ret .= $this->region."\n";
    }
    $ret .= $this->postal_code.' '.strtoupper($this->city)."\n";
    $ret .= strtoupper(H::country($this->country_id, $locale));
    if (!empty($this->information)) {
      $ret .= "\n".$this->information;
    }
    if (strlen($this->phone) >= 6) {
      $ret .= "\n".$this->phone;
    }
    
    return $ret;
  }


}