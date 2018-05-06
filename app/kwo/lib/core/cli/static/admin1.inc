<?php

$table = 'knx_admin1';

$file_path = 'http://download.geonames.org/export/dump/admin1CodesASCII.txt';

$countries = $dbo->asHash('SELECT code, id FROM '.T('country'));

$handle = fopen($file_path, 'r');

if ($handle === false) {
  throw new Exception('can not open file');
}

$dbo->getTable($table)->truncate();

$regions = array();

while (($data = fgetcsv($handle, 1000, "\t")) !== false) {
  list ($iso_code, $code) = explode('.', $data[0]);
  if ($countries[$iso_code] < 1) continue ;
  $name = $data[1];
  if ($countries[$iso_code] == 250) {
    if ($name == 'Brittany') $name = 'Bretagne';
    elseif ($name == 'Corsica') $name = 'Corse';
    $regions[(string)$code] = $name;
  }
  $dbo->insert($table,
               array('id' => $data[3],
                     'country_id' => $countries[$iso_code],
                     'code' => sprintf('%u', crc32($code)),
                     'name' => $name)); 
  
}
fclose($handle);

$dbo->getTable($table)->optimize();

ksort($regions);
print(var_export($regions, true));

/*$rewrites = array(3030293 => 'Bretagne',
                  3023519 => 'Corse');

foreach ($rewrites as $key => $value) {
  $dbo->update($table,
               array('name' => $value),
               array('filters' => array('id' => $key),
                     'limit' => 1));
}

$dbo->query('UPDATE knx_admin1 SET name=REPLACE(name, "Region ", "") WHERE country_id='.Country::FR);
$dbo->query('UPDATE knx_admin1 SET name=REPLACE(name, "Région ", "") WHERE country_id='.Country::FR);
*/

/*

CREATE TABLE IF NOT EXISTS `knx_admin1` (
  `id` mediumint(8) unsigned NOT NULL default '0',
  `code` int(10) unsigned NOT NULL default '0',
  `country_id` smallint(5) unsigned NOT NULL default '0',
  `name` varchar(255) NOT NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`country_id`,`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

*/