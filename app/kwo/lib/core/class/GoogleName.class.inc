<?php

class GoogleName{

  const TABLE = 'core_google_name';

  const TYPE_CITY = 1;
  const TYPE_ADMIN1 = 2;
  const TYPE_ADMIN2 = 3;


  public static function id($component, $type, $args) {
    $locale = (int) $args['locale'] > 0 ? (int) $args['locale'] : H::locale();
    $country_id = (int) $args['country_id'] > 0 ? (int) $args['country_id'] : Country::FR;
    if (empty($component)) return 0;
    $id = (int) S('dbo')->fetchValue('SELECT id FROM '.self::TABLE
                                     .' WHERE locale=#locale#'
                                     .  ' AND type=#type#'
                                     .  ' AND name=:name:',
                                     array('#locale#' => $locale,
                                           '#type#' => $type,
                                           ':name:' => $component['long_name']));
    if ($id > 0) return $id;
    $record = array('country_id' => $args['country_id'],
                    'locale' => $args['locale'],
                    'name' => trim($component['long_name']),
                    'type' => $type);
    $id = S('dbo')->insert(self::TABLE, $record);
    return $id;
  }

  public static function name($id, $locale) {
    $name = S('dbo')->fetchValue('SELECT name FROM '.self::TABLE
                                 .' WHERE id='.intval($id)
                                 .  ' AND locale='.intval($locale));
    return $name;
  }

}