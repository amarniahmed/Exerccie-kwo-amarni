<?php

try {
  
  $rs = $dbo->query('SELECT j.user_id, j.privilege_id, j.available_from, j.available_to,'
                    .     ' u.name '
                    .' FROM '.T('user').' AS u,'
                    .     ' '.User::TABLE_PRIVILEGE.' AS j'
                    .     ' LEFT JOIN '.T('person').' AS p ON p.user_id=j.user_id'
                    .' WHERE j.user_id=u.id'
                    .' ORDER BY j.available_from DESC'); 
  
  $columns = array('user_id', 'privilege_id', 'available_from', 'available_to', 'user', 'company');

  $opts = array('columns' => $columns,
                'encoding' => 'UTF-8',
                'filename' => 'abonnements-'.DH::today());
  $csv = CsvWriter::getInstance($opts);

  if ($rs === false || $rs->numRows() < 1) return ;
  
  while ($row = $rs->getNext()) {
    $line = $row;
//    $line[] = $user_name;
    $csv->append($line);
  }

}
catch (Exception $e) { $err->add($e); }
