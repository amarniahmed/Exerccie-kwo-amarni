<?php

class Merchant extends ActiveRecord {

  public function asAddress() {
    $ret .= '<b>'.$this->corporate_name.'</b>'."\n";
    if (!empty($this->caption)) { $ret .= '<i>'.$this->caption.'</i>'."\n"; }
    $ret .= $this->address."\n";
    if (!empty($this->address_extra)) {
      $ret .= $this->address_extra."\n";
    }
    $ret .= $this->postal_code.' '.$this->city."\n";
    $ret .= $this->country."\n";
    if (!empty($this->phone)) { $ret .= 'Tel : '.$this->phone."\n"; }
    if (!empty($this->fax)) { $ret .= 'Fax : '.$this->fax."\n"; }
    if (!empty($this->email)) { $ret .= $this->email."\n"; }
    return $ret;
  }

  public static function getInstanceByApp() {
    $app = S('app')->getName();
    $args = array('app' => $app);
    $id = Merchant::id($args);
    if ($id < 1) {
      $id = (int) S('dbo')->fetchValue('SELECT id FROM '.T('merchant'));
    }
    $merchant = Merchant::getInstance($id);
    return $merchant;
/*    try {
      $args = array('app' => $app);
      $merchant = Merchant::getInstance($args);
      return $merchant;
    }
    catch (Exception $e) {
      S('log')->error('please create a merchant for app ['.$app.']', __METHOD__);
      $id = (int) S('dbo')->fetchValue('SELECT id FROM '.T('merchant'));
      return  Merchant::getInstance($id);
    } */
  }

}
