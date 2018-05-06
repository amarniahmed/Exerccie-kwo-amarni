<?php

try {
  
//  if (!($req->manual == 'yes' || intval(date('H')) === 3)) return ;

//  $filters = null;
//  if (true) {
    $filters = array();
//    $filters['|DAY|created_at'] = DH::yesterday();
    $filters['|>|visit_id'] = 0;
    $filters['visit_number'] = 0;
//  }
  $opts = array('filters' => $filters);
  foreach (Conversion::collect($opts) as $conversion) {
    if ($conversion->visit_id < 1) continue ;
    $attrs = array();
    try {
      $visit = Visit::getInstance($conversion->visit_id);
      $attrs['visit_number'] = $visit->number;
      if ($visit->number <= 1) {
        $attrs['span'] = 0;
/*        echo join(' ', array($conversion->id, $visit->visitor_id, '('.$visit->number.')', $vis->created_on, 
                             $conversion->created_at, $attrs['span']))."\n"; */
      }
      else {
        $visit_id = $dbo->fetchValue('SELECT id FROM '.Visit::TABLE
                                     .       ' WHERE visitor_id='.$visit->visitor_id
                                     .    ' ORDER BY id ASC');
        if ($visit_id < 1 || $visit_id == $conversion->visit_id) continue ;
        $vis = Visit::getInstance($visit_id);
        $d1 = new DateTime($conversion->created_at);
        $d2 = new DateTime($vis->created_on);
        $interval = $d1->diff($d2, true);
        $attrs['span'] = $interval->format('%a');
        
      }
      $conversion->setAttributes($attrs);
    }
    catch (Exception $e) { $err->log($e); }
  }

}
catch (Exception $e) { $err->add($e); }
