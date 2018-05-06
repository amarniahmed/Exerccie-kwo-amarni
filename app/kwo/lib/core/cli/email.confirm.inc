<?php

// return User::getInstance(1)->requestEmailConfirmation();

$now = DH::now();

$rs = $dbo->query('SELECT u.*, DATEDIFF("'.$now.'", md.updated_at) AS day_count'
                  .' FROM '.T('user').' AS u,'
                  .     ' '.Meta::TABLE.' AS md'
                  .' WHERE md.'.Meta::FIELD_MODEL.'='.Model::id('user')
                  .  ' AND md.'.Meta::FIELD_RECORD.'=u.id'
                  .  ' AND u.status='.User::STATUS_PENDING
                  .' HAVING day_count in (7, 15, 90, 182, 365, 730)');

if ($rs->numRows() < 1) return ;

foreach ($rs->asCollection('user') as $user) {
  $log->debug($user->email.' ('.$user->id.')');
  $user->requestEmailConfirmation();
}
