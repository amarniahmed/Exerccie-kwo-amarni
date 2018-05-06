<?php

try {
  
  $hour = (int) date('H');

  if ($hour != 6) return ;

  $limits = array(2 => (6 * 31),
                  3 => (12 * 31));

  foreach ($limits as $events => $days) {
    
    $ts = mktime(0, 0, 0, date('m'), date('d') - $days, date('Y'));
    $date = date('Y-m-d', $ts);
    
    $dbo->exec('DELETE FROM '.Tracker::TABLE_HIT
               .' WHERE visit_id IN (SELECT id FROM '.Visit::TABLE
                                           .' WHERE created_on="'.$date.'"'
                                             .' AND conversions=0'
                                             .' AND depth<='.$events.')');

    //    S('log')->trace('numrows : '.$dbo->affectedRows());

  }

}
catch (Exception $e) { $err->add($e); }
