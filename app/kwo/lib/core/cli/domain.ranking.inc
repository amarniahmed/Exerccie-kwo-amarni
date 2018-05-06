<?php

$rs = $dbo->query('SELECT DISTINCT REVERSE(LEFT(REVERSE(email), LOCATE("@", REVERSE(email)) - 1)) AS domain,'
                  .     ' COUNT(*) AS n'
                  .' FROM '.T('user')
                  .' GROUP BY domain'
                  .' ORDER BY n DESC'
                  .' LIMIT 100');

foreach ($rs as $row) {
  echo str_pad($row['domain'], 32).$row['n']."\n";
}