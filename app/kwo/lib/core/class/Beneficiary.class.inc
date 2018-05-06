<?php

class Beneficiary extends ActiveRecord {

  const LINK_SPOUSE = 1;
  const LINK_CHILD = 2;

  public static $links = array(self::LINK_SPOUSE => 'conjoint',
                               self::LINK_CHILD => 'enfant');

}