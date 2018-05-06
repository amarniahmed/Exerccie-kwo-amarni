<?php

class GoogleArea {

  const TABLE = 'core_google_area';


  public static function record($json, $args) {
    $json = json_decode($json, true);
    $locale = $args['locale'] ?: H::locale();
    $country_id = $args['country_id'] ?: Country::FR;
    $parts = array();
    $res = $json['results'][0];

    if (is_array($res['address_components'])) {
      foreach ($res['address_components'] as $component) {
        switch ($component['types'][0]) {
          case 'country':
            $country_id = Country::id(array('code' => $component['short_name']));
            break;
          case 'postal_code': $parts['postal_code'] = $component; break;
          case 'locality': $parts['city'] = $component; break;
          case 'administrative_area_level_1': $parts['admin1'] = $component; break;
          case 'administrative_area_level_2': $parts['admin2'] = $component; break;
        }
      }

      $args = array('country_id' => $country_id,
                    'locale' => $locale);

      $record = array('admin1_id' => GoogleName::id($parts['admin1'], GoogleName::TYPE_ADMIN1, $args),
                      'admin2_id' => GoogleName::id($parts['admin2'], GoogleName::TYPE_ADMIN2, $args),
                      'city_id' => GoogleName::id($parts['city'], GoogleName::TYPE_CITY, $args),
                      'country_id' => $country_id,
                      'postal_code' => $parts['postal_code']['long_name']);
print_r($record);
      S('dbo')->replace(self::TABLE, $record);
    }
  }

}