<?php

class Geolocation extends ActiveRecord {

  public static function geocode($item, $opts=null) {
    $id = Geolocation::id($item);
    $geolocation = Geolocation::getInstance($id >= 1 ? $id : true);

    if ($opts['address']) {
      $param = 'address='.urlencode($opts['address']);
    }
    elseif ($opts['latitude'] && $opts['longitude']) {
      $param = 'latlng='.trim($opts['latitude']).','.trim($opts['longitude']);
    }
    elseif (Model::hasFields($item, array('address', 'city')) &&
            !empty($item->address) && !empty($item->city)) {
      $param = 'address='.urlencode(self::asAddress($item));
    }

    $table = 'core_google_cache';
    $url = 'http://maps.googleapis.com/maps/api/geocode/json?sensor=false&language=en&'.$param;

    $h = S('dbo')->fetchHash('SELECT response, id FROM '.$table.' WHERE url=:url:',
                         array(':url:' => $url));

   if (empty($h)) {
      $response = $json = file_get_contents($url);
      $json = json_decode($json, true);
      if ($json['status'] == 'OK') {
        S('dbo')->insert($table, array('url' => $url, 'response' => $response));
      }
   }
   else {
     $json = $h['response'];
     $json = json_decode($json, true);
     S('dbo')->query('UPDATE '.$table.' SET COUNT=COUNT+1 WHERE id='.$h['id']);
    }


    $res = $json['results'][0];

    $row = array('model_id' => $item->getModelId(),
                 'record_id' => $item->getId(),
                 'address' => $res['formatted_address'],
                 'latitude' => $res['geometry']['location']['lat'],
                 'locale' => I18N::LOCALE_EN,
                 'longitude' => $res['geometry']['location']['lng'],
                 'name' => $item->getName(),
                 'zoom' => ($opts['zoom'] ?: 16));

    $fields = array('admin1', 'admin2', 'admin3', 'country', 'establishment',
                    'locality', 'postal_code', 'sublocality');

    foreach ($fields as $field) {
      $row[$field] = '';
    }
    foreach (array('country_id', 'radius', 'type') as $field) {
      $row[$field] = 0;
    }

    if (is_array($res['address_components'])) {
      foreach ($res['address_components'] as $component) {
        switch ($component['types'][0]) {
          case 'postal_code':
            $row['postal_code'] = $component['long_name'];
            break ;
          case 'locality':
            $row['locality'] = $component['long_name'];
            break;
          case 'sublocality':
            $row['sublocality'] = $component['long_name'];
            break;
          case 'country':
            $row['country'] = $component['long_name'];
            $row['country_id'] = Country::id(array('code' => $component['short_name']));
            break;
          case 'administrative_area_level_1':
            $row['admin1'] = $component['long_name'];
            break;
          case 'administrative_area_level_2':
            $row['admin2'] = $component['long_name'];
            break;
          case 'administrative_area_level_3':
            $row['admin3'] = $component['long_name'];
            break;
        }
      }
    }

    $geolocation->commit($row);

    $metas = $attrs = array();

    if ($item->hasAttributes('latitude', 'longitude')) {
      $metas[Meta::FIELD_LATITUDE] = $row['latitude'];
      $metas[Meta::FIELD_LONGITUDE] = $row['longitude'];
      $attrs['latitude'] = $row['latitude'];
      $attrs['longitude'] = $row['longitude'];
    }

    foreach (array('admin1', 'admin2', 'country_id') as $key) {
      if ($item->hasAttribute($key)) {
        $attrs[$key] = $row[$key];
      }
    }

    if (!empty($metas)) {
      $item->setMetas($metas);
    }

    if (!empty($attrs)) {
      $item->setAttributes($attrs);
    }

  }

  public static function asAddress($item) {
    if (method_exists($item, 'asGeolocationAddress')) {
      return $item->asGeolocationAddress();
    }
    $parts = array();
    $parts[] = $item->address;
    if (!empty($item->address_extra)) {
      $parts[] = ' '.$item->address_extra;
    }
    if (!empty($item->postal_code)) {
      $parts[] = ', '.$item->postal_code;
      $parts[] = ' '.$item->city;
    }
    else {
      $parts[] = ', '.$item->city;
    }
    if ($item->country_id > 0) {
      $parts[] = ', '.Country::Name($item->country_id);
    }
    return join('', $parts);
  }

}
