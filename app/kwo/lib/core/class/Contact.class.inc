<?php

class Contact extends ActiveRecord {

  const FLAG_DIR = 1;
  const FLAG_COM = 2;
  const FLAG_RH = 4;

  public static $civilities = array(0 => 'm.',
                                    1 => 'mme',
                                    2 => 'mlle');

  public static $statuses = array(1 => 'en poste',
                                  0 => 'indéfini',
                                  -1 => 'parti');

  public static $types = array(1 => 'direction',
                               2 => 'marketing',
                               3 => 'commercial',
                               4 => 'finance',
                               5 => 'juridique',
                               6 => 'rh',
                               0 => 'indéfini');

  protected function onBeforeCommit(&$hash) {
    if (isset($hash['first_name']) && isset($hash['last_name'])) {
      $hash['name'] = $hash['first_name'].' '.$hash['last_name'];
    }
  }

}