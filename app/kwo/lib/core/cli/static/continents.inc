<?php

$c_arr = $dbo->asHash('select code, id from geo_continent');

$handle = fopen('continents.txt', 'r');
while (($data = fgetcsv($handle, 1000, "\t")) !== FALSE) {
  $dbo->update(T('country'), 
               array('continent_id' => $c_arr[$data[1]]),
               array('filters' => array('code' => $data[0])));
}
fclose($handle);
