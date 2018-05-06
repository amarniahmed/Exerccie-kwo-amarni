<?php

class Residence extends ActiveRecord {

  /**
   * Retourne l'adresse complète
   *
   * @return string
   */
  public function asAddress() {
    $parts = array();

    $parts[] = $this->address;
    $parts[] = $this->address_extra;
    $parts[] = $this->postal_code;
    $parts[] = $this->city;
    $parts[] = Country::name($this->country_id);

    return join(' ', $parts);
  }

}
