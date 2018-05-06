<?php

try {

  $filters = array('|>|status' => 0,
                   '|%|code' => 'reminder.order.client.');
  $opts = array('filters' => $filters);

  foreach (Letter::collect($opts) as $letter) {
    list (,, $months) = explode('.', $letter->code);
    $months = (int) $months;
    if ($months < 1) continue ;
    $rs = $dbo->query('SELECT u.id'
                      .' FROM '.T('user').' AS u,'
                      .     ' '.User::TABLE_INFORMATION.' AS i'
                      .' WHERE i.last_order_date>0'
                      .  ' AND (TO_DAYS(:now:) - TO_DAYS(o.last_order_date))='.($months*30)
                      .  ' AND i.user_id=u.id'
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