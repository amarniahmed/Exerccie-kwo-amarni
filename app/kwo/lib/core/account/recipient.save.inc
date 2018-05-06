<?php

try {

  if (empty($req->email)) return ;

  if (!Email::isValid($req->email, true)) {
    throw new Exception(l('error.email.invalid'));
  }

  $recipient_id = Email::asId($req->email);
  
  $old = $user->getInformation('recipient_id');

  if ($recipient_id >= 1 && $recipient_id != $old) {
    $bases = $dbo->asSet('SELECT base_id'
                         .' FROM '.Base::TABLE_EMAIL
                         .' WHERE email_id='.$recipient_id);
    $rows = $dbo->asArray('SELECT * FROM '.Base::TABLE_EMAIL
                          .      ' WHERE email_id='.$old);
    foreach ($rows as $row) {
      if (in_array($row['base_id'], $bases)) {
        $dbo->exec('DELETE FROM '.Base::TABLE_EMAIL
                   .    ' WHERE base_id='.$row['base_id']
                   .      ' AND email_id='.$row['email_id']
                   .' LIMIT 1');
        $dbo->trace();
      }
      else {
        $dbo->exec('UPDATE '.Base::TABLE_EMAIL
                   .' SET email_id='.$recipient_id
                   .' WHERE email_id='.$old
                   .  ' AND base_id='.$row['base_id']
                   .' LIMIT 1');
      }
    }
    $user->setInformation('recipient_id', $recipient_id);
  }

  $res->setFlash(l('recipient.saved'));

}
catch (Exception $e) { $err->add($e); }
