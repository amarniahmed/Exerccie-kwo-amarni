<?php

// http://fr.wikipedia.org/wiki/Code_postal_en_France

function cleaner($name, $country_id) {
  //  $name = utf8_decode($name);
  if ($country_id == Country::FR) {
    $name = str_ireplace(array('Département du ', 
                               'Département des ', 
                               'Département de l\'', 
                               'Département de la ', 
                               'Département de ', 
                               'Département d\''),
                        array('', '', '', '', '', ''),
                        $name);
  }
  return $name;
}

//FR.B3.31	Département de la Haute-Garonne	Departement de la Haute-Garonne	3013767
//FR.A5.2B	Département de la Haute-Corse	Departement de la Haute-Corse	3013793

$file_path = 'http://download.geonames.org/export/dump/admin2Codes.txt';

$countries = $dbo->asHash('SELECT code, id FROM '.T('country'));

$rs = $dbo->query('SELECT id, code, country_id'
                   .' FROM knx_admin1');
$admin1 = array();
foreach ($rs as $row) {
  $admin1[$row['country_id']][$row['code']] = $row['id'];
}

$handle = fopen($file_path, 'r');

if ($handle === false) {
  throw new Exception('can not open file');
}

$dbo->getTable('knx_admin2')->truncate();

$france = array('FR', 'MQ', 'GP', 'GF', 'RE');

while (($data = fgetcsv($handle, 1000, "\t")) !== false) {
  list($iso_code, $admin1_code, $code) = explode('.', $data[0]);
  if ($code == $data[3] || strpos($code, ' ') !== false || $code == $data[1]) {
    $code = '';    
  }
  else {
    $code = sprintf('%u', crc32($code));
    //    $code = (int) $code;
  }
  //  if (strlen($code) > 7) { S('log')->debug($data); }
  //  $code = sprintf('%u', crc32($code));
  //  $code = (int) $code;
  if (in_array($iso_code, $france)) {
    $country_id = Country::FR;
  } 
  else {
    $country_id = (int) $countries[$iso_code];
  }
  if ($country_id < 1) continue ;
  $admin1_code = sprintf('%u', crc32($admin1_code));
  $admin1_id = $admin1[$country_id][$admin1_code];
  $city = cleaner($data[1], $country_id);
  //  $city = $data[1];
  $dbo->insert('knx_admin2',
               array('id' => $data[3],
                     'country_id' => $country_id,
                     'admin1_id' => $admin1_id,
                     'code' => $code,
                     'name' => $city)); 
}
fclose($handle);

$dbo->getTable('knx_admin2')->optimize();

/*

CREATE TABLE IF NOT EXISTS `knx_admin2` (
  `id` mediumint(8) unsigned NOT NULL default '0',
  `admin1_id` mediumint(8) unsigned NOT NULL default '0',
  `code` int(10) unsigned NOT NULL default '0',
  `country_id` smallint(5) unsigned NOT NULL default '0',
  `name` varchar(255) collate latin1_general_ci NOT NULL,
  PRIMARY KEY  (`id`),
  KEY `admin1_id` (`admin1_id`),
  KEY `country_id` (`country_id`),
  KEY `code` (`code`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci ROW_FORMAT=DYNAMIC;

*/