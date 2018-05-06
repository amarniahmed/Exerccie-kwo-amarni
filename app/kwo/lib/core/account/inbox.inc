<?php

try { 
  
  $rs = $dbo->query('SELECT SQL_CALC_FOUND_ROWS m.*, md.created_at, mu.status,'
                    .     ' IFNULL(u2.name, u.name) AS user_name,'
                    .     ' IFNULL(u2.path, u.path) AS user_path,'
                    .     ' IFNULL(u2.flags, u.flags) AS user_flags'
                    .' FROM '.Message::TABLE_RECIPIENTS.' AS mu,'
                    .     ' '.Meta::TABLE.' AS md,'
                    .     ' '.T('user').' AS u,'
                    .     ' '.T('message').' AS m'
                    .     ' LEFT JOIN '.T('user').' AS u2 ON (m.replier_id=u2.id)'
                    .' WHERE mu.user_id='.$user->getId()
                    .  ' AND mu.message_id=m.id'
                    .  ' AND mu.status > 0'
                    .  ' AND m.user_id=u.id'
                    .  ' AND md.'.Meta::FIELD_MODEL.'='.Model::id('message')
                    .  ' AND md.'.Meta::FIELD_RECORD.'=mu.message_id'
                    .' ORDER BY mu.message_id DESC'
                    .' LIMIT '.Message::INBOX_LIMIT
                    .' OFFSET '.($req->offset * Message::INBOX_LIMIT));

  if ($req->offset >= 1 && $rs->numRows() < 1) {
    $res->sendRedirect('inbox');
    return ;
  }

  $ctx->messages = $rs;
  $ctx->offset = (int) $req->offset;

  $res->setMenu('inbox');
  $res->setTitle(l('account.inbox'));

}
catch (Exception $e) {
  $log->error($e);
  $err->add($e); 
}

