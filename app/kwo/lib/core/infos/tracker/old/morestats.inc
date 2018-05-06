<?php 

$year  = (int) $req->year;
$month = (int) $req->month;
$day   = (int) $req->day;

$rs = $dbo->query('SELECT qty_arr, navig_arr, sys_arr, host_arr'
                  .' FROM tkr_cache'
                  .' WHERE year='.$year
                  .  ' AND month='.$month
                  .  ' AND day='.$day);
$obj = $rs->fetchObject();

$qty_arr = AH::decode($obj->qty_arr);
$navig_arr = AH::decode($obj->navig_arr);
$sys_arr = AH::decode($obj->sys_arr);

$hosts = AH::decode($obj->host_arr);

$qty_arr['user'] = (int) $qty_arr['user'];
$qty_arr['visit_five'] = (int) $qty_arr['visit_five'];

?>

<table class="layout" style="margin-top:6px;"><tr class="tdLeft"><td>

<?php
$duration = $qty_arr['duration'] / ($qty_arr['visit'] - $qty_arr['visit_one']);
$list_arr['durée moyenne'] = ceil($duration / 60).' mn';
if ($qty_arr['custom'] >= 1) {
  $list_arr['fréquence des venues'] = ceil($qty_arr['span'] / $qty_arr['custom']).' j';
}
if (($qty_arr['visit'] - $qty_arr['visit_one']) > 0) {
  $list_arr['Nbre pages / visite significative'] = $n = round($qty_arr['page'] / ($qty_arr['visit'] - $qty_arr['visit_one']));
  if ($n > 0) {
    $list_arr['tps moyen / page'] = round($duration / $n).' s';
  }
}
$list_arr['visites identifiées'] = round($qty_arr['user'] * 100 / $qty_arr['visit'], 1).' % ('.$qty_arr['user'].')';
$list_arr['nb robot'] = $qty_arr['robot'];
$list_arr['visites avec 5p ou +'] = round(($qty_arr['visit_five'] / $qty_arr['visit']) * 100, 1).' %'.' ('.$qty_arr['visit_five'].')';
$list_arr['tx visites en soirée'] = round(($qty_arr['night'] / $qty_arr['visit']) * 100, 1).' %';
if ($obj->day==0) $list_arr['tx visites week-end'] = round(($qty_arr['we'] / $qty_arr['visit']) * 100, 1).' %';

$i = 0;
print("<table class='grid auto'>\n");
print("<thead><th colspan='2' style='width:100%'>Infos</th></tr><tbody>\n");
foreach ($list_arr as $k => $v) print('<tr><td>'.$k.'</td><td>'.$v.'</td></tr>'."\n");
print("</tbody></table>\n");

if (count($hosts) > 0) {
  arsort($hosts);
  $keys = array_keys($hosts);
  $names = $dbo->asHash('SELECT id, name FROM tkr_string WHERE id IN ('.join(',', $keys).')');
  $i = 0;
  print("<table class='grid auto'>\n");
  print("<thead><th colspan='2' style='width:100%'>Domaines</th></tr><tbody>\n");
  foreach ($hosts as $key => $value) {
    print('<tr><td>'.$value.'</td><td>'.$names[$key].'</td></tr>'."\n");
  }
  print("</tbody></table>\n");
}

?>

</td><td class='tdRight'>

<?php

$a = array();
$arr = $sys_arr['browser'];
$arr = array_slice($arr, 0, 5, true);
$i = 0;
$clients = IH::$browsers;
print("<table class='grid auto'>\n");
print("<thead><th colspan='2'>Navigateurs</th></thead><tbody>\n");
foreach ($arr as $k => $v) {
  print('<tr><td style="width:40px;">'.round(($v/$qty_arr['visit'])*100,1).'%</td>');
  print('<td>'.$clients[$k].'</td></tr>'."\n");
  $a[$clients[$k]] = round(($v / $qty_arr['visit']), 2);
}
print("</tbody></table>\n");

print('<img src="http://chart.apis.google.com/chart?cht=p3&chd=t:'.join(',', array_values($a)).'&chs=280x100&chl='.join('|', array_keys($a)).'" />');

$a = array();
$arr = $sys_arr['system'];
$arr = array_slice($arr, 0, 8, true);
$i = 0;
$clients = IH::$systems;
print("<table class='grid auto'>\n");
print("<thead><th colspan='2'>Systèmes</th></thead><tbody>\n");
foreach ($arr as $k => $v) {
  print('<tr><td style="width:40px;">'.round(($v/$qty_arr['visit'])*100,1).'%</td>');
  print('<td>'.$clients[$k].'</td></tr>'."\n");
  $a[$clients[$k]] = round(($v / $qty_arr['visit']), 2);
}
print("</tbody></table>\n");

print('<img src="http://chart.apis.google.com/chart?cht=p3&chd=t:'.join(',', array_values($a)).'&chs=280x100&chl='.join('|', array_keys($a)).'" />');

$arr_out = array_slice($navig_arr['out'], 0, 5, true);
$arr_in = array_slice($navig_arr['in'], 0, 5, true);
$names_arr = $dbo->asHash('SELECT id, name FROM tkr_string WHERE id IN ([ids])',
                          array('[ids]' => array_merge(array_keys($arr_in), array_keys($arr_out))));
$i = 0;
print("<table class='grid auto' style='margin-top:10px;'>\n");
print("<thead><th colspan='2'>Points d'entrée</th></thead><tbody>\n");
foreach ($arr_in as $k => $v) {
  print('<tr><td style="width:40px;">'.round((($v)/$qty_arr['visit'])*100,1).'%</td>');
  print('<td>'.$names_arr[$k].'</td></tr>'."\n");
}
print("</tbody></table>\n");

$i = 0;
print("<table class='grid auto' style='margin-top:10px;'>\n");
print("<thead><th colspan='2'>Points de sortie</th></thead><tbody>\n");
foreach ($arr_out as $k => $v) {
  print('<tr><td style="width:40px;">'.round((($v)/$qty_arr['visit'])*100,1).'%</td>');
  print('<td>'.$names_arr[$k].'</td></tr>'."\n");
}
print("</table>\n");

?>

</td></tr></table>