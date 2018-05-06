<?php

$handle = fopen('c.txt', 'r');
while (($data = fgetcsv($handle, 1000, "\t")) !== FALSE) {
  $dbo->insert('shop_currency', 
               array('id' => $data[1],
                     'code' => $data[0],
                     'name' => $data[3]));
}
fclose($handle);
