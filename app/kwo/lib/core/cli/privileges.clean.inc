<?php

$rs = $dbo->query('SELECT privilege_id, user_id'
                  .' FROM '.User::TABLE_PRIVILEGE
                  .' WHERE available_to > 0'
                  .  ' AND available_to < "'.DH::now().'"');

foreach ($rs as $row) {
  if (!User::exists($row['user_id'])) {
    $dbo->exec('DELETE FROM '.User::TABLE_PRIVILEGE
               .    ' WHERE user_id='.$row['user_id']);
    continue ;
  }
  $user = User::getInstance($row['user_id']);
  $user->unsetFlag(NH::asBit($row['privilege_id']), 'privileges');
  $dbo->exec('DELETE FROM '.User::TABLE_PRIVILEGE
             .    ' WHERE privilege_id = '.$row['privilege_id']
             .      ' AND user_id = '.$user->getId());
}