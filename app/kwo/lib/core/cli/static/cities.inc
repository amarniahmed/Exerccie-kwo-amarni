<?php

if (!file_exists('cities.txt')) {

  // cities1000, cities5000, cities15000
  $file = 'cities1000';

  $cmd = 'wget -nv -q "http://download.geonames.org/export/dump/'.$file.'.zip"';
  Shell::exec($cmd);
  
  if (!file_exists($file.'.zip')) {
    die('ZIP not found');
  }
  
  $cmd = 'unzip -qq '.$file.'.zip';
  Shell::exec($cmd);
  
  unlink($file.'.zip');
  
  rename($file.'.txt', 'cities.txt');

}

if ($dbo->hasTable('knx_geoname')) { 
  $dbo->exec('DROP TABLE knx_geoname');
}

if (!$dbo->hasTable('knx_city')) { 
  $dbo->exec("CREATE TABLE IF NOT EXISTS `knx_city` (
  `id` mediumint(8) unsigned NOT NULL,
  `admin1_id` mediumint(8) unsigned NOT NULL default '0',
  `admin2_id` mediumint(8) unsigned NOT NULL default '0',
  `country_id` smallint(5) unsigned NOT NULL default '0',
  `name` varchar(255) NOT NULL,
  `population` int(10) unsigned NOT NULL default '0',
  `latitude` double NOT NULL default '0',
  `longitude` double NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `city` (`name`,`admin1_id`,`country_id`),
  KEY `admin1_id` (`admin1_id`),
  KEY `admin2_id` (`admin2_id`),
  KEY `country_id` (`country_id`)
)");
}

$dbo->exec("CREATE TABLE IF NOT EXISTS knx_geoname (
  `id` int(10) unsigned NOT NULL,
  `name` varchar(255) collate utf8_general_ci NOT NULL,
  `latitude` double NOT NULL,
  `longitude` double NOT NULL,
  `featureclass` varchar(1) collate utf8_general_ci NOT NULL,
  `featurecode` varchar(10) collate utf8_general_ci NOT NULL,
  `countrycode` varchar(2) collate utf8_general_ci NOT NULL,
  `admin1` varchar(20) collate utf8_general_ci NOT NULL,
  `admin2` varchar(80) collate utf8_general_ci NOT NULL,
  `population` int(10) unsigned NOT NULL,
  `admin1_code` int(10) unsigned NOT NULL default '0',
  `admin2_code` int(10) unsigned NOT NULL default '0',
  KEY featurecode (featurecode,countrycode)
)");


$dbo->exec("LOAD DATA LOCAL INFILE './cities.txt' INTO TABLE knx_geoname (id, @d, name, @d, latitude,  longitude, featureclass, featurecode, countrycode, @d,  admin1, admin2, @d, @d, population, @d, @d, @d, @d);");

$dbo->exec('UPDATE knx_geoname SET admin1_code=CRC32(admin1), admin2_code=CRC32(admin2)');

$dbo->exec('TRUNCATE TABLE knx_city');

$dbo->exec("INSERT IGNORE INTO knx_city"
           ." SELECT g.id,"
           .      " knx_admin1.id,"
           .      " knx_admin2.id,"
           .      " C.id,"
           .      " g.name,"
           .      " g.population,"
           .      " g.latitude,"
           .      " g.longitude"
           ." FROM knx_geoname AS g"
           .     " JOIN ".T('country')." AS C ON (C.code=g.countrycode)"
           .     " LEFT JOIN knx_admin1 ON (knx_admin1.country_id=C.id AND knx_admin1.code=g.admin1_code)"
           .     " LEFT JOIN knx_admin2 ON (knx_admin2.country_id=C.id AND knx_admin2.code=g.admin2_code)"
           //           ." WHERE featureclass='P' AND featurecode NOT IN ('PPLQ', 'PPLW') ORDER BY g.id");
           ." WHERE featureclass='P'"
           .  " AND countrycode='FR'"
           .  " AND featurecode NOT IN ('PPLQ', 'PPLW')"
           ." ORDER BY g.id");


if ($dbo->hasTable('knx_geoname')) { 
  $dbo->exec('DROP TABLE knx_geoname');
}

$dbo->getTable('knx_city')->optimize();

unlink('cities.txt');


//$dbo->debug();

//$dbo->exec('DROP TABLE knx_geoname');

