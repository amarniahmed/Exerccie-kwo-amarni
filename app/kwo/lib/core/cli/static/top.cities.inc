<?php

try {

  $codes = array();
  foreach (France::$departements as $key => $a) {
    $codes[$a[1]] = $key;
  }

//print_r($codes); return ;

  $rs = $dbo->query('SELECT a.name,'
                    .     ' GROUP_CONCAT(c.name ORDER BY c.population DESC) as names,'
                    .     ' AVG(c.latitude) AS lat,'
                    .     ' AVG(c.longitude) AS lng'
                    .' FROM knx_city AS c, knx_admin2 AS a'
                    .' WHERE c.admin2_id=a.id AND c.country_id=250'
                    .  ' AND c.population>10000'
                    .' GROUP BY c.admin2_id');

  foreach ($rs as $row) {
    $code = $codes[$row['name']] ?: $row['name'];
    array_shift($row);
//    array_pop($row);
    $row['names'] = preg_replace("/'/u", "\'", $row['names']);
//    print("'".$code."' => array('".join("','", $row)."'),\n");
  }


}
catch (Exception $e) { $err->add($e); }
