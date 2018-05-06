<?php

try {

  $filters = array('|>|status' => 0,
                   '|%|code' => 'reminder.order.pending.');
  $opts = array('filters' => $filters);

  foreach (Letter::collect($opts) as $letter) {
    list (,, $days) = explode('.', $letter->code);
    $days = (int) $days;
    if ($days < 1) continue ;
    $rs = $dbo->query('SELECT o.user_id'
                      .' FROM '.T('order').' AS o,'
                      .     ' '.T('user').' AS u'
                      .' WHERE o.user_id>0'
                      .  ' AND o.status='.Order::STATUS_NONE
                      .  ' AND (TO_DAYS(:now:) - TO_DAYS(o.checkedout_at))='.$days
                      .  ' AND o.user_id=u.id'
                      .  ' AND u.status>0',
                      array(':now:' => DH::now()));
    foreach ($rs as $row) {
      $user = User::getInstance($row['user_id']);
      $recipient = $user;
//      $recipient = 'fxbois@kernix.com';
      Letter::send($letter->code, $recipient);
    }
  }

}
catch (Exception $e) { $err->add($e); }
