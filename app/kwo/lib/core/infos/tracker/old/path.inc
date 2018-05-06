<?php

$obj = $dbo->fetchObject('SELECT navig_arr'
                         .' FROM tkr_cache'
                         .' WHERE year=:y: AND month=:m: AND day=:d:',
                         array(':y:' => $req->year,
                               ':m:' => $req->month,
                               ':d:' => $req->day));
if ($obj === false) {
  return ;
}

$arr = AH::decode($obj->navig_arr);

if (!isset($arr['path'])) return ;

$arr = array_slice($arr['path'], 0, 10 , true);

$names_arr = $dbo->asHash('SELECT id, name FROM tkr_string WHERE id IN (#ids#)',
                          array('#ids#' => join(',', array_keys($arr))));

foreach ($arr as $event_set => $nb) {
  $events_arr = explode(',', $event_set);
  print("<div style='border-bottom:1px solid #ccc; padding: 6px 0 6px 0'>");
  foreach ($events_arr as $i => $id) {
    if ($i >= 1) {
      print(' <img src="/app/tracker/pix/arrow.gif" style="margin:0 3px 0 3px;" /> ');
    }
    print($names_arr[$id]);
  }
  print("</div>\n");
}