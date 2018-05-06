<?php

// street, city, state, postal code, country

/*****
 const CIVILITY_M = 0; // m. mister
 const CIVILITY_MME = 1; // mme madame
 const CIVILITY_MLE = 2; // mlle miss
*****/


class Person extends ActiveRecord {

  public static $civilities = array(0 => 'm.',
                                    1 => 'mme',
                                    2 => 'mlle');

  public function validate(&$hash) {
    $n = 0;
    if (isset($hash['website'])) {
      $hash['website'] = trim($hash['website'], '/');
      if (stripos($hash['website'], 'http') !== 0) {
        $hash['website'] = 'http://'.$hash['website'];
      }
      $hash['website'] = Valid::isUrl($hash['website']) ? $hash['website'] : '';
    }
    if (isset($hash['description'])) {
      $hash['description'] = strip_tags($hash['description']);
      $hash['description'] = str_replace(array("\n", "\r", '"'),
                                         array(' ', ' ', "'"),
                                         $hash['description']);
    }
    foreach ($hash as $key => &$value) {
      $value = trim($value);
      if ($key != 'website') {
        $value = c($value);
      }
      $value = SH::clean($value);
      //      if (!empty($value)) $n++;
    }
    //    if ($n < 1) throw new ProfileException('empty profile');
    if ($this->dbo->getTable($this->_table)->hasFields(array_keys($hash)) === false) {
      S('log')->error('bad profile field : '.join(', ', array_keys($hash)), __METHOD__);
      throw new ProfileException('invalid profile field');
    }
    if (!empty($hash['postal_code']) && !empty($hash['city']) &&
        preg_match('/^(\d){3,}$/', $hash['city'])) {
      $tmp = $hash['postal_code'];
      $hash['postal_code'] = $hash['city'];
      $hash['city'] = $tmp;
    }

    /***

    utiliser l'attribut $errors de ProfileException() pour propager les erreurs
    ex:
    $errors['phone'] = 'numéro de tel invalide';
    $e = new ProfileException('au moins un champs est invalide');
    $e->errors = $errors;
    throw $e;

    ***/

  }

  public function asEditHash() {
    return array();
  }

  public function asAddress($locale=null) {
    $ret = '';
    $ret .= ucfirst($this->first_name).' '.ucfirst($this->last_name)."\n";
    $ret .= $this->address."\n";
    if (strlen($this->address_extra) >= 2) {
      $ret .= $this->address_extra."\n";
    }
    $ret .= $this->postal_code.' '.strtoupper($this->city)."\n";
    $ret .= strtoupper(H::country($this->country_id, $locale));
    if (strlen($this->phone) >= 6) {
      $ret .= "\n".$this->phone;
    }
    if (strlen($this->mobile) >= 6) {
      $ret .= "\n".$this->mobile;
    }
    return $ret;
  }

  public function getCompleteness() {
    $fields = array('address', 'city', 'country_id', 'first_name',
                    'last_name', 'phone', 'postal_code');
    $n = 0;
    foreach ($fields as $field) {
      $value = $this->getAttribute($field);
      if (empty($value) || $value == 0) continue ;
      $n++;
    }
    return ceil(($n / count($fields)) * 100);
  }

  public function identity() {
    return ucfirst($this->first_name).' '.ucfirst($this->last_name);
  }


}