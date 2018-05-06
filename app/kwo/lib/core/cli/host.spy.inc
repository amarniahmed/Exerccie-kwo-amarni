<?php

try {

  $spacer = '    ';

  $rs = $dbo->query('SELECT ip,'
                    .     ' COUNT(id) AS n,'
                    .     ' GROUP_CONCAT(created_at ORDER BY created_at SEPARATOR " ") as t'
                    .' FROM '.Visit::TABLE
                    .' WHERE created_on="'.date('Y-m-d').'"'
                    .' GROUP BY ip'
                    .' HAVING n >= 2'
                    .' ORDER BY n DESC');
  foreach ($rs as $row) {
    $ip = IH::num2ip($row['ip']);
    $str  = $row['n'].' : ';
    $str .= $ip."\n";
    $str .= $spacer.$row['ip']."\n";
    $str .= $spacer.gethostbyaddr($ip)."\n";
    $str .= $spacer.$row['t']."\n";
    echo $str."\n";
  }

}
catch (Exception $e) { $err->add($e); }
