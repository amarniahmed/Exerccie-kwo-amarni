<?php

try {

  $periods = array(7 => 'une semaine',
                   30 => 'un mois');

  $now = DH::now();

  $rs = $dbo->query('SELECT u.*,'
                    .     ' DATEDIFF(p.available_to, "'.$now.'") AS day_count'
                    .' FROM '.T('user').' AS u,'
                    .     ' '.User::TABLE_PRIVILEGE.' AS p'
                    .' WHERE p.user_id = u.id'
                    .  ' AND u.status >= '.User::STATUS_PENDING
                    .' HAVING day_count in (7, 30)');
  
  if ($rs->numRows() < 1) return ;

  $body_tpl = 'la licence va expirer dans ((period))';

  foreach ($rs->asCollection('user') as $user) {
    //    $log->debug($user->email.' ('.$user->id.')');
    $mail = Mail::getInstance('mail');
    $mail->addRecipient($user);
    $mail->setSubject('attention licence');
    $body = str_replace('((period))', 
                        $periods[intval($user->day_count)],
                        $body_tpl);
    $mail->setBody($body, true);
    $mail->send();
  }

}
catch (Exception $e) { $err->add($e); }

