<?php

try {

  $rs = $dbo->query('SELECT u.id'
                    .' FROM '.T('delivery').' AS d,'
                    .     ' '.T('user').' AS u'
                    .' WHERE TO_DAYS(d.opened_at)=TO_DAYS("'.DH::yesterday().'")'
                    .  ' AND d.user_id=u.id'
                    .  ' AND u.status='.User::STATUS_PENDING);
  foreach ($rs as $row) {
    $user = User::getInstance((int) $row['id']);
    $user->setStatus(User::STATUS_CONFIRMED);
  }

}
catch (Exception $e) { $err->add($e); }
