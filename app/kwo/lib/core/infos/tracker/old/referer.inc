<?php

$max = 50;

$year  = (int) $req->year;
$month = (int) $req->month;
$day   = (int) $req->day;

$rs = $dbo->query('SELECT referer_arr'
                  .' FROM tkr_cache'
                  .' WHERE year='.$year
                  .  ' AND month='.$month
                  .  ' AND day='.$day);
$obj = $rs->fetchObject();

$referer_arr = (array) @unserialize($obj->referer_arr);

$key_arr = (array) $referer_arr[$req->medium]['source'];
$tag_arr = (array) $referer_arr[$req->medium]['term'];


$key_ids_arr = array_keys($key_arr);
$tag_ids_arr = array_keys($tag_arr);

$names_arr = $dbo->asHash('SELECT id, name FROM tkr_string WHERE id IN ([ids])',
                          array('[ids]' => (array_merge($key_ids_arr, $tag_ids_arr)))); 


print("<table class='layout'><tr><td class='tdLeft'>\n");

$i = 1;
print("<table class='grid auto'>\n");
print("<thead><th colspan='2'>origine</th></thead><tbody>\n");
foreach ($key_arr as $k => $v) print("<tr".(($i++%2)?' class="alt"':'')."><td style='width:14%;'>".$v."</td><td>".$names_arr[$k]."</td></tr>\n");
print("</tbody></table>\n");

print("</td><td class='tdRight'>\n");

if (count($tag_arr) > 1) {
  $i = 1;
  print("<table class='grid auto'>\n");
  print("<thead><th colspan='2'>étiquette</th></thead><tbody>\n");
  foreach ($tag_arr as $k => $v) {
    $percent = round(($v/$referer_arr[$req->medium]['qty'])*100,1).'%';
    print("<tr".(($i++%2)?' class="alt"':'')."><td style='width:15%;' title='".$v." visite(s)'>".$percent."</td>");
    print("<td>".substr($names_arr[$k], 0, 40)."</td></tr>\n");
  }
  print("</tbody></table>\n");
}

print("</td></tr></table>\n");

