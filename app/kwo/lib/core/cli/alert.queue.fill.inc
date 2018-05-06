<?php

try {

  $rs = $dbo->query('SELECT a.id, a.search_id, a.user_id,'
                    .     ' u.model_id, u.record_id'
                    .' FROM '.T('alert').' AS a,'
                    .     ' '.Search::TABLE_UPDATES.' AS u'
                    .' WHERE a.frequency='.Alert::FREQ_DAY
                    .  ' AND a.status>0'
                    .  ' AND a.search_id=u.search_id'
                    .  ' AND u.added_on=:yesterday:'
                    .' ORDER BY a.user_id',
                    array(':yesterday:' => DH::yesterday()));

  $table = $dbo->getTable(Alert::TABLE_QUEUE);
  foreach ($rs as $row) {
    $row = array('alert_id' => $row['id'],
                 'model_id' => $row['model_id'],
                 'record_id' => $row['record_id'],
                 'user_id' => $row['user_id']);
    if ($table->hasRow($row)) continue ;
    $dbo->insert(Alert::TABLE_QUEUE, $row);
  }

  S('app')->hook('onAlertMemoryUpdate');

}
catch (Exception $e) { $err->add($e); }