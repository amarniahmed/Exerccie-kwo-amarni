<?php

try {

  $where = '1';

  $visit_id = $req->visit_id ?: 56073;

  $visit = Visit::getInstance((int) $visit_id);
  $where .= ' AND visitor_id='.$visit->visitor_id;

  $rs = $dbo->query('SELECT id,visitor_id FROM '.T('visit').' WHERE '.$where.' ORDER BY id DESC LIMIT 5');

  $opts = array('separator' => ',');
  $res->sendData($rs, $opts);

}
catch (Exception $e) { $err->add($e); }
