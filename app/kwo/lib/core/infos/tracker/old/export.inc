<?php

try {

  $obj = TrackerManager::obj();
  $qty_arr = AH::decode($obj->qty_arr);
  
  $res->sendData('stdout://', 
                 array('filename' => 'export.csv',
                       'charset' => 'x-UTF-16LE-BOM'));
  
  $taux = (($qty_arr['visit'] > 0) ? round((($qty_arr['custom']/$qty_arr['visit'])*100),1) : '0').' %';
  
  $string = "Statistiques - ".$obj->period_name."
visites\t".$qty_arr['visit']."
pages vues\t".$qty_arr['page']."
habitués\t".$taux."

Origine
";
  
  $referer_arr = AH::decode($obj->referer_arr);
  foreach (Tracker::$mediums as $k => $v) {
    $val = ($qty_arr['visit'] > 0) ? round((($referer_arr[$k]['qty'])/$qty_arr['visit'])*100,1) : 0;
    $name = $v;
    $string .= $name."\t".$val."\n";
  }
  
  $res->write("\xff\xfe".
              mb_convert_encoding($string, 'UTF-16LE', 'UTF-8'));

}
catch (Exception $e) { $err->add($e); }
