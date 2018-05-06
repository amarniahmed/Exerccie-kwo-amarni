<?php

try { 
  
  $conditions = '';
  $user_id = (int) $req->getBranchValue('user_id');
  if ($user_id > 0) {
    $conditions = ' AND u.id='.$user_id;
  }

  $rs = $dbo->query('SELECT p.name AS privilege_name,'
                    .     ' pu.available_from,'
                    .     ' pu.available_to,'
                    .     ' u.*'
                    .' FROM '.User::TABLE_PRIVILEGE.' AS pu,'
                    .   ' '.T('privilege').' AS p,'
                    .   ' '.T('user').' AS u'
                    .' WHERE pu.available_to > 0'
                    .  ' AND pu.privilege_id = p.id'
                    .  ' AND pu.user_id = u.id'
                    .$conditions
                    .' ORDER BY pu.available_to ASC'
                    .' LIMIT 200');
  
  $ctx->rs = $rs;

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
