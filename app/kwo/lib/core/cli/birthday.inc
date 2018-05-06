<?php

try {

  $code = 'birthday';

  $id = Letter::id($code);
  if ($id < 1) return ;
  $letter = Letter::getInstance($id);
  if (!$letter->isOnline()) return ;

  if (true) {
    $where = 'MONTH(i.birth_date)='.date('m').' AND DAY(i.birth_date)='.date('d');
  }
  else {
    $where = '(TO_DAYS("'.DH::now().'") - TO_DAYS(i.birth_date)) = 10';
  }

  $rs = $dbo->query('SELECT u.*'
                    .' FROM '.User::TABLE_INFORMATION.' AS i,'
                    .     ' '.T('user').' AS u'
                    .' WHERE '.$where
                    .  ' AND i.user_id=u.id'
                    .  ' AND u.status>='.User::STATUS_PENDING); 
  $users = $rs->asCollection('user');
  foreach ($users as $user) {
//    if ($user->hasNotice()) continue ;
    Letter::send($code, $user);
  }

}
catch (Exception $e) { $err->add($e); }
