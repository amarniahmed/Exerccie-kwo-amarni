<?php

try { 

  $base = Base::getInstance($req->id);

  $fields = '';
  if (false) {
    $fields = ', CONCAT_WS("-", e.id, SUBSTRING(MD5(CONCAT(id, "'.P('app.seed').'")), -8)) AS eauth';
  }

  $rs = $dbo->query('SELECT e.email, s.status'.$fields
                    .' FROM '.Base::TABLE_EMAIL.' AS s,'
                    .     ' '.T('email').' AS e'
                    .' WHERE s.email_id=e.id'
                    .  ' AND s.base_id='.$base->getId());
  
  $res->sendData($rs);

}
catch (Exception $e) { $err->add($e); }
