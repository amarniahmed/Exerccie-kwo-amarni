<?php 

try {
  
  $rs = $dbo->query('SELECT qty_arr, geo_arr FROM tkr_cache WHERE year=:y: AND month=:m: AND day=:d:',
                    array(':y:'=>$req->year,':m:'=>$req->month,':d:'=>$req->day));
  $period = $rs->fetchObject();
  
  $qty_arr = AH::decode($period->qty_arr);
  $geo_arr = AH::decode($period->geo_arr);
  $geo_arr = SH::asSet($geo_arr['country']);
  $geo_arr = array_slice($geo_arr, 0, 50, true);
  
  $country_arr = $dbo->asHash('SELECT id, code, name_fr'.
                              ' FROM '.T('country')
                              .' WHERE id IN ([geo_arr])',
                              array('[geo_arr]' => array_keys($geo_arr)), 
                              'id');
  
  $grid_arr = array();
  foreach ($geo_arr as $k => $v) {
    $cel = array();
    $cel['country_flag'] = '<img src="/'.PIX_PATH.'/flags/'.strtolower($country_arr[$k]['code']).'.png" />';
    $cel['value'] = $v;
    $cel['percent'] = round(($v / $qty_arr['visit']) * 100, 1).'%';
    $cel['country_name'] = $country_arr[$k]['name_fr'];
    $grid_arr[] = $cel;
  }
  
  $ctx->grid_arr = $grid_arr;
  
  $res->useXml();
}
catch (Exception $e) {}