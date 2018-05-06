<?php

// */10 * * * * root cd /var/web/proj && php init.php order.warning

return ;

try {

  $min = 10;
  $max = 20;

  $rs = $dbo->query('SELECT o.id, o.total_amount, o.user_id, u.email'
                    .' FROM '.T('order').' AS o,'
                    .     ' '.T('user').' AS u'
                    .' WHERE o.user_id>0'
                    .  ' AND o.status='.Order::STATUS_NONE
                    .  ' AND TIMESTAMPDIFF(MINUTE, o.checkedout_at, :now:) BETWEEN '.$min.' AND '.$max
                    .  ' AND o.user_id=u.id'
                    .  ' AND u.status>=0',
                    array(':now:' => DH::now())); $dbo->debug();
  $opts = array('login' => 'xx',
                'password' => 'yy',
                'from' => 'zz');
  foreach ($rs as $row) {
    $user = User::getInstance($row['user_id']);
    $profile = $user->getProfile();
    $smsc = SmsClient::getInstance($opts);
    $smsc->addRecipient('06XX');
    $phone = $profile->mobile ?: $profile->phone;
    $msg = $row['email']."\n".'Tel. '.$phone."\n".'cmde('.$row['id'].') '.$row['total_amount'].'EUR';
    $smsc->send($msg);
    echo $msg . "\n";
  }
  
}