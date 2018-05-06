<?php

$from = $req->from;
$to   = $req->to;

if (empty($from)) {
  $from = date('Y-m-01');
  $to   = date('Y-m-d');
}

if (!empty($from) && empty($to)) $to = $from;

list($year, $month, $day) = explode('-',$from);
$year  = (int) $year;
$month = (int) $month;
$day   = (int) $day;

$chunks = explode('-', $to);
$to = intval($chunks[0]).'-'.intval($chunks[1]).'-'.intval($chunks[2]);

if ($year==0) {
  $title = 'Statistiques globales';
  $where = 'year=0 AND month=0 AND week=0 AND day=0';
}
elseif ($month==0) {
  $title = "Statistiques de l'année ".$year;
  $where = 'year='.$year.' AND month=0 AND week=0 AND day=0';
}
elseif ($day==0) {
  $title = 'Statistiques du mois de '.I18N::$months[I18N::LOCALE_FR]['long'][$month].' '.$year;
  $where = 'year='.$year.' AND month='.$month.' AND day=0';
}
else {
  if ($from != $to) $title = "Statistiques sur la période du $from au $to";
  else $title = "Statistiques de la journée du $from";
  $where = "day>0 AND equiv_date>='".$from."' AND equiv_date<='".$to."'";
}

$obj_arr = array();
$rs = $dbo->query('SELECT * FROM tkr_cache WHERE '.$where.' ORDER BY equiv_date');
while ($obj = $rs->fetchObject()) {
  $obj->qty_arr     = (array) @unserialize($obj->qty_arr);
  $obj->chapter_arr = (array) @unserialize($obj->chapter_arr);
  $obj->referer_arr = (array) @unserialize($obj->referer_arr);
  $obj->lead_arr    = (array) @unserialize($obj->lead_arr);
  $obj->navig_arr   = (array) @unserialize($obj->navig_arr);
  $obj->sys_arr  = (array) @unserialize($obj->sys_arr);
  $obj_arr[] = $obj;
}

$days_arr[] = array('jour','nb visites','nb visiteurs');

$qty_arr = $chapter_arr = $referer_arr = $lead_arr = $navig_arr = $geo_arr = $sys_arr = array();
$hour_arr = $day_arr = $week_arr = $month_arr = $year_arr = array();

foreach ($obj_arr as $obj) {
  $days_arr[] = array($obj->equiv_date,$obj->qty_arr['visit'],$obj->qty_arr['visitor']);
  $res_arr['nb visiteurs']           += $obj->qty_arr['visitor'];
  $res_arr['nb visites']             += $obj->qty_arr['visit'];
  $res_arr['nb pages']               += $obj->qty_arr['page'];
  $res_arr['tx habitués (%)']        += $obj->qty_arr['custom'];
  $res_arr['tx visite à 1 page (%)'] += $obj->qty_arr['visit_one'];
  $res_arr['tx visites de nuit (%)'] += $obj->qty_arr['night'];
  $res_arr['durée moyenne (mn)']     += $obj->qty_arr['duration'];
  $qty_arr     = AH::merge($qty_arr,$obj->qty_arr);
  $chapter_arr = AH::merge($chapter_arr,$obj->chapter_arr);
  $referer_arr = AH::merge($referer_arr,$obj->referer_arr);
  $lead_arr    = AH::merge($lead_arr,$obj->lead_arr);
  $navig_arr   = AH::merge($navig_arr,$obj->navig_arr);
  $sys_arr  = AH::merge($sys_arr,$obj->sys_arr);
}

//$chapter_arr  = $this->conv($chapter_arr);
//$referer_arr  = $this->conv($referer_arr);
if (count($obj_arr)>=1) {
  $res_arr['durée moyenne (mn)']     = ceil(($res_arr['durée moyenne (mn)']/$res_arr['tx habitués (%)'])/60);
  $res_arr['tx visite à 1 page (%)'] = round((($res_arr['tx visite à 1 page (%)']/$res_arr['nb visites'])*100),1);
  $res_arr['tx habitués (%)']        = round((($res_arr['tx habitués (%)']/$res_arr['nb visites'])*100),1);
  $res_arr['tx visites de nuit (%)'] = round((($res_arr['tx visites de nuit (%)']/$res_arr['nb visites'])*100),1);
}

if ($req->format=='excel') {
  $excel = new Excel();
  $sheet_arr = array();
  foreach ($res_arr as $k => $v) $sheet_arr[] = array($k,$v);
  $excel->addWorksheet('divers',$sheet_arr);

  $sheet_arr = array();
  foreach ($chapter_arr as $k => $v) {
    $sheet_arr[] = array('__'.$k,$v['qty']);
    foreach ($v['title_arr'] as $title => $qty) $sheet_arr[] = array($title,$qty);
    $sheet_arr[] = array('','');
  }
  $excel->addWorksheet('chapitres',$sheet_arr);

  $sheet_arr = array();
  foreach (Tracker::$mediums as $k => $v) {
    $qty = (int) $referer_arr[$k]['qty'];
    $sheet_arr[] = array('__'.$v,$qty);
    $tmp_arr = (array) $referer_arr[$k]['source'];
    foreach ($tmp_arr as $key => $qty) $sheet_arr[] = array($key,$qty);
    $sheet_arr[] = array('','');
  }
  $excel->addWorksheet('sources',$sheet_arr);

  $sheet_arr = array();
  foreach (Tracker::$lead_type_arr as $k => $v) {
    $qty = (int) $lead_arr[$k]['qty'];
    $sheet_arr[] = array('__'.$v,$qty);
    $tmp_arr = (array) $lead_arr[$k]['medium'];
    foreach ($tmp_arr as $grp => $qty) $sheet_arr[] = array(Tracker::$mediums[$grp],$qty);
    $sheet_arr[] = array('','');
  }
  $excel->addWorksheet('leads',$sheet_arr);

  unset($sheet_arr);
  if (count($days_arr) > 2) {
    $excel->addWorksheet('par jour',
                         $days_arr);
  }
  $res->sendData('mem://export-tracker.xls', 
                 array('content' => $excel->getContent()));
  return;
} 

?>

<br/>

<div style="width:200px;float:left; padding:0 20px 0 20px;" id="div_date" class="noprint">
<form action="/back/tracker/fromto" autocomplete="off" id="form">
 <input type="hidden" id="nb_line" value="<?=count($obj_arr)?>" />
 Depuis (aaaa-mm-dd) <input type="text" name="from" id="from" value="<?=$from?>" /> <br/>
 Jusqu à (aaaa-mm-dd) <input type="text" name="to" id="to" value="<?=$to?>" /> <br/>
<img src="/<?=PIX_PATH?>/action/search.png" class="button" onClick="$('form').submit()" />
<img src="/<?=PIX_PATH?>/action/print.png"  class="button" onClick="printStat()" />
<img src="/<?=PIX_PATH?>/action/export.png" class="button" onClick="statExport()" />
<img src="/<?=PIX_PATH?>/action/back.png" class="button" onClick="statBack()" />
</form>
</div>


<style>
TR.subTR TD { border-top:1px solid #ddd; }
</style>

<div style="width:600px; float:left;">

<?php

if (count($obj_arr)<1) return print('Cette période ne contient aucune donnée</div>');

?>

<table class="grid">
<tr><th colspan="2"><?=$title?></th></tr>
<?php
foreach ($res_arr as $k => $v) print("<tr".(($i++%2)?' class="alt"':'')."><td width=40%>".$k."</td><td>".$v."</td></tr>");
?>
</table>
<br/><br/>

<table class="grid auto">
<tr><th colspan="2">Chapitres</th></tr>
<?php
foreach ($chapter_arr as $chapter => $arr) {
  print("<tr".(($i++%2)?' class="alt"':'')."><td width=30%>".$chapter.' ('.$arr['qty'].')</td><td>');
  print("<table class='grid auto'>\n");
  foreach ($arr['title_arr'] as $k => $v) {
    $percent = round($v*100/$arr['qty'],1);
    print("<tr".(($j++%2)?' class="alt"':'')."><td width=18%>".$v.'</td><td style="width:18%; border-right:1px #ddd solid">'.$percent." %</td><td>".$k."</td></tr>");
  }
  print("</table>\n");
  print("</td></tr>");
}
?>
</table>
<br/><br/>

<table class="grid auto">
<tr><th colspan="2">Sources</th></tr>
<?php
foreach (Tracker::$mediums as $k => $v) {
  print("<tr".(($i++%2)?' class="alt"':'')."><td style='width:20%;'>".$v.' ('.((int)$referer_arr[$k]['qty']).')</td>');
  print('<td>');
  print("<table class='grid auto'>\n");
  $key_arr = (array) $referer_arr[$k]['source'];
  foreach ($key_arr as $key => $qty) {
    print("<tr".(($j++%2)?' class="alt"':'')."><td width=20%>".$qty."</td><td>".$key."</td></tr>");
  }
  print("</table>\n");
  print("</td>");
  print('<td style="border-left:1px #ddd solid">');
  print("<table class='grid auto'>\n");
  $tag_arr = (array) $referer_arr[$k]['term'];
  foreach ($tag_arr as $tag => $qty) {
    print("<tr".(($j++%2)?' class="alt"':'')."><td width=20%>".$qty."</td><td>".$tag."</td></tr>");
  }
  print("</table>\n");
  print("</td>");
  print("</tr>\n");
}
?>
</table>
<br/><br/>

<table class="grid auto">
<tr><th colspan="2">Lead</th></tr>
<?php
foreach (Tracker::$lead_type_arr as $k => $v) {
  print("<tr".(($i++%2)?' class="alt"':'')."><td style='width:20%;'>".$v.' ('.((int)$lead_arr[$k]['qty']).')</td>');
 print('<td>');
  print("<table class='grid auto'>\n");
  $grp_arr = (array) $lead_arr[$k]['medium'];
  foreach ($grp_arr as $grp => $qty) {
    print("<tr".(($j++%2)?' class="alt"':'')."><td width=20%>".$qty."</td><td>".Tracker::$mediums[$grp]."</td></tr>");
  }
  print("</table>\n");
  print("</td>");
  print('<td style="border-left:1px #ddd solid">');
  print("<table class='grid auto'>\n");
  $key_arr = (array) $lead_arr[$k]['source'];
  foreach ($key_arr as $key => $qty) {
    print("<tr".(($j++%2)?' class="alt"':'')."><td width=20%>".$qty."</td><td>".$key."</td></tr>");
  }
  print("</table>\n");
  print("</td>");
  print('<td style="border-left:1px #ddd solid">');
  print("<table class='grid auto'>\n");
  $tag_arr = (array) $lead_arr[$k]['term'];
  foreach ($tag_arr as $tag => $qty) {
    print("<tr".(($j++%2)?' class="alt"':'')."><td width=20%>".$qty."</td><td>".$tag."</td></tr>");
  }
  print("</table>\n");
  print("</td>");
  print("</tr>\n");
}
?>
</table>
<br/><br/>

</div>
