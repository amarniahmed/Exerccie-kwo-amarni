<?php

try { 

  $rs = $dbo->query('SELECT e.email AS email,'
                    .     ' e.id AS email_id,'
                    .     ' a.created_at'
                    .' FROM '.Act::TABLE.' AS a, '
                    .       T('email').' AS e'
                    .' WHERE a.email_id=e.id'
                    .  ' AND a.campaign_id='.intval($req->id)
                    .  ' AND a.type='.intval($req->type));

  $res->sendData($rs, array('filename' => 'emails'));

}
catch (Exception $e) { $err->add($e); }