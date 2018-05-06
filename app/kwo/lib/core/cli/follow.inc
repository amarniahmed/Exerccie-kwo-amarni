<?php

try {

  $rs = $dbo->query('SELECT c.model_id, c.record_id, md.created_at,'
                    .     ' COUNT(c.record_id) AS n,'
                    .     ' f.user_id, u.email, u.locale'
                    .' FROM '.T('comment').' as c,'
                    .     ' core_follow AS f,'
                    .     ' '.T('user').' AS u,'
                    .     ' '.Meta::TABLE.' AS md'
                    .' WHERE c.record_id=f.record_id'
                    .  ' AND c.model_id=f.model_id'
                    .  ' AND md.'.Meta::FIELD_MODEL.'='.Model::id('comment')
                    .  ' AND md.'.Meta::FIELD_RECORD.'=c.id'
                    .  ' AND md.created_at > DATE_SUB("'.DH::now().'", interval 1 HOUR)'
                    .  ' AND f.status=1'
                    .  ' AND u.id=f.user_id'
                    .  ' AND u.status>=1'
                    .' GROUP BY c.model_id, c.record_id, f.user_id'
                    .' ORDER BY c.model_id, c.record_id'); 

  if ($rs->numRows() < 1) return ;

  $sending_count = 0;

  $remove = H::url('/follow.remove/-/item/');

  foreach ($rs as $hash) {

    // TODO : uid ?
    if ($hash['n'] == 1 && $hash['uid'] == $hash['user_id']) {
      continue ;
    }

    $item = Item::getInstance($hash['model_id'],
                              $hash['record_id']);

    Letter::send('discussion.update', $hash['email'], 
                 array('link' => $item->asUrl(),
                       'remove' => $remove.$item->getKey(),
                       'title' => $item),
                 array('locale' => $hash['locale']));

    $sending_count++;

  }
  
  if ($sending_count > 50) {
    $log->trace('sending_count ('.$sending_count.')');
  }

}
catch (Exception $e) { $log->error($e); }