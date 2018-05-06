<?php

$max = 100; 

$rs = $dbo->query("SELECT qty_arr,chapter_arr"
                  ." FROM tkr_cache"
                  ." WHERE year='".$req->year."' AND month='".$req->month."' AND day='".$req->day."'");
$obj = $rs->fetchObject();

$qty_arr = AH::decode($obj->qty_arr);
$chapter_arr = AH::decode($obj->chapter_arr);

$ids_arr = array_keys($chapter_arr);
foreach ($chapter_arr as $k => $v) {
  $ids_arr = array_merge($ids_arr, array_keys($v['title_arr']));
}

$names_arr = $dbo->asHash('SELECT id, name FROM tkr_string WHERE id IN ([ids])',
                          array('[ids]' => $ids_arr));

$events_labels = array('events' => 'evenements : site',
                       'social.events' => 'evenements : réseau social',
                       'search' => 'recherche',
                       'errors' => 'erreurs',
                       'shop' => 'boutique',
                       'account' => 'espace membre',
                       'auth' => 'authentification',
                       'search.empty' => 'recherche vide');

$categories = array();
$i = 0;
foreach ($chapter_arr as $k => $v) {
  $label = $names_arr[$k];
  if (isset($events_labels[$label])) {
    $label = $events_labels[$label];
  }
  $categories[] = array('code' => $names_arr[$k],
                      'label' => $label,
                      'count' => $v['qty'],
                      'index' => $i);
  $i++;
}

function cmp($a, $b)
{
    return strcoll($a['label'], $b['label']);
}

usort($categories, "cmp");

$n = count($chapter_arr);

print('<script src="/app/tracker/back.js" type="text/javascript"></script>');
print("<div class='divCat-1' width='100%' style='display:block'>");
print("<table width='100%' class='grid auto'>");
print("<thead><th colspan='3'>catégories</th></thead><tbody>");

$j = 0;
foreach ($categories as $hash) {
  print("<tr>");
  print("<td style='width:14%;text-align:left;'>".$hash['count']."</td>");  
  print("<td style='border-left: 1px #ddd solid'>");
  print("<a href='javascript:void(0)' onClick='Kwo.Stats.onToggle(this, ".$hash['index'].")'>".$hash['label']."</a>");
  print("</td>");
  print("</tr>\n");
  $i++;
}

print("</tbody></table>");
print("</div>\n");

$i = 0;
foreach ($chapter_arr as $k => $v) {
  print("<div class='divCat$i' width='100%' style='display:".(($i==-1)?'block':'none')."'><table width='100%' class='grid auto'>");
  print("<thead><th colspan='3'><a href='javascript:void(0)' onClick='Kwo.Stats.onToggle(this, -1)'>");
  print($names_arr[$k]." (".$chapter_arr[$k]['qty'].")</a></th></thead><tbody>\n");
  $j = 0;
  foreach ($v['title_arr'] as $a => $w) {
    $val = $w * 100 / $chapter_arr[$k]['qty'];
    $percent = round($val, 1).'%';
    print("<tr>");
    print("<td style='width:14%;text-align:left;'>".$w."</td>");
    print("<td style='width:10%;text-align:left;'>".$percent."</td>");
    print("<td style='border-left:1px #ddd solid'>".$names_arr[$a]."</td>");
    print("</tr>\n");
  }
  print("</tbody></table></div>\n");
  $i++;
}
