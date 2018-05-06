<?php

try {
  
  $limit = '';
  if ($req->limit == 1) {
    $limit = ' LIMIT '.intval($req->limit_value);
  }

  $where = '1';
  
  if (strlen($req->from) > 5) {
    $where .= ' AND u.registered_at >= "'.DH::toDb($req->from).'"';
  }

  if (strlen($req->to) > 5) {
    $where .= ' AND u.registered_at <= "'.DH::toDb($req->to).'"';
  }

  $cols = 'u.id, u.status, u.email, u.name, u.registered_at';

  $from = T('user').' AS u';
  if ($req->profile) {
    $cols .= ', p.*';
    $from .= ' LEFT JOIN '.T($req->profile).' AS p ON p.user_id=u.id';
    $where .= ' AND p.user_id > 0';
  }

  $rs = $dbo->query('SELECT '.$cols.' FROM '.$from
                    .' WHERE '.$where
                    .' ORDER BY u.id DESC'
                    .$limit); 
  
//  $columns = array('user_id', 'status', 'email', 'name', 'registered_at');

  $filename = 'users-';
  if ($req->profile) {
    $filename .= $req->profile.'-';
  }
  $filename .= DH::today();

  $opts = array('columns' => true,
                //'columns' => array('user_id', 'status', 'email', 'name', 'registered_at'),
                'encoding' => 'UTF-8',
                'filename' => $filename);
  $csv = CsvWriter::getInstance($opts);
  
  if ($rs === false || $rs->numRows() < 1) return ;
  
  while ($row = $rs->getNext()) {
    $csv->append($row);
  }

}
catch (Exception $e) { $err->add($e); }
