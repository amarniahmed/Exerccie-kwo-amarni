<?php

$table = T('country');

/*$handle = fopen('ces.txt', 'r');
while (($data = fgetcsv($handle, 1000, "\t")) !== FALSE) {
  $dbo->update($table, 
               array('name_de' => strtoupper($data[1])),
               'code=:code:',
               array(':code:' => $data[0]));
}
fclose($handle);*/

/*$handle = fopen('cnfr.txt', 'r');
while (($data = fgetcsv($handle, 1000, ";")) !== FALSE) {
  print_r($data);
  $dbo->update($table, 
               array('name_en' => $data[0]),
               'code=:code:',
               array(':code:' => $data[1]));
}
fclose($handle);*/

/*
$handle = fopen('countries.txt', 'r');
while (($data = fgetcsv($handle, 1000, "\t")) !== FALSE) {
  $dbo->update($table, 
               array('id' => $data[0],
                     'code' => $data[2],
                     'iso3_code' => $data[1]),
               'code2=:code:',
               array(':code:' => $data[2]));
}
fclose($handle);
*/

/*$handle = fopen('countries2.txt', 'r');
$i = 0;
while (($data = fgetcsv($handle, 1000, "\t")) !== FALSE) {
  $i++;  
  if ($i === 1) continue;
  if (strlen($data[5]) == 2) {
    $dbo->update($table, 
                 array('fips_code' => $data[5]),
                 'code=:code:',
                 array(':code:' => $data[1]));
  }
  else {
    print($data[0].'->'.$data[1].'->'.$data[5]."\n");
  }
}

fclose($handle);*/