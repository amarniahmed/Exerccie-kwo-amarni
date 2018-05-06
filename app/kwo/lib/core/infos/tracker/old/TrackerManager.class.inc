<?php

class TrackerManager
{

  public static function obj() {
    $req = S('req');
    $year = (int) $req->year;
    $month = (int) $req->month;
    $day = (int) $req->day;
    $rs = S('dbo')->query('SELECT * FROM tkr_cache WHERE year='.$year.' AND month='.$month.' AND day='.$day);
    $obj = $rs->fetchObject();
    $obj->period_name = 'Depuis le lancement du site';
    if ($day >= 1) {
      $obj->period_name = 'Journée - '.$day.'/'.$month.'/'.$year;
    }
    elseif ($month >= 1) {
      $obj->period_name = 'Mois - '.$month.'/'.$year;
    }
    elseif ($year >= 1) {
      $obj->period_name = 'Année - '.$year;
    }
    return $obj;
  }

  public static function renderBloc($year,$month,$day) {
    
    $dbo = S('dbo');
    
    $months = I18N::$months[I18N::LOCALE_FR]['long'];

    $opts = array();
    if ($day>=1) {
      $type = 'day';
      $rs = $dbo->query("SELECT year,month,day FROM tkr_cache WHERE year=".$year." AND month=".$month." AND week<>0 AND day<>0 ORDER BY day");
      while ($obj = $rs->fetchObject()) $opts[$obj->day] = $obj->day.' '.$months[$obj->month].' '.$obj->year;
      $sql = "SELECT * FROM tkr_cache WHERE year=".$year." AND month=".$month." AND day=".$day;
      $sel_value = $day;
    }
    elseif ($month>=1) {
      $type = 'month';
      $rs = $dbo->query("SELECT year,month FROM tkr_cache WHERE year=".$year." AND month<>0 AND week=0 AND day=0 ORDER BY month");
      while ($obj=$rs->fetchObject()) $opts[$obj->month] = $months[$obj->month].' '.$obj->year;
      $sql = "SELECT * FROM tkr_cache WHERE year=".$year." AND month=".$month." AND week=0 AND day=0";
      $sel_value = $month;
  }
  elseif ($year>=1) {
    $type = 'year';
    $rs = $dbo->query("SELECT year FROM tkr_cache WHERE year<>0 AND month<>0 AND week=0 AND day=0 ORDER BY year");
    while ($obj=$rs->fetchObject()) $opts[$obj->year] = $obj->year;
    $sql = "SELECT * FROM tkr_cache WHERE year=".$year." AND month=0 AND week=0 AND day=0";
    $sel_value = $year;
  }
  else {
    $type = 'global';
    $opts[]   = "statistiques depuis l&#39;origine";
    //    $opts[-3] = "date à date";
    $sql = "SELECT * FROM tkr_cache WHERE year=0 AND month=0 AND week=0 AND day=0";
    $sel_value = '';
  }
  
  $rs = $dbo->query($sql);
  $obj = $rs->fetchObject();
    
  $qty_arr = AH::decode($obj->qty_arr);
  
  $query  = 'year='.$obj->year.'&month='.$obj->month.'&day='.$obj->day;
  $iframe = "iframeStat".ucfirst($type);
  
  $html = '';
  $html .= "<select style='width:51%;float:left;margin:0;' id='select-".$type."' onchange=\"Kwo.Stats.onExec(this)\" data-action=\"load\">";
  foreach ($opts as $k => $v) {
    $html .= " <option value='".$k."'".(($sel_value==$k)?' selected="selected"':'').">".$v."</option>\n";
  }
  $html .= "</select>";

  $html .=  "<div style='text-align:right;clear:right;'>";
  if ($type=='day' && $qty_arr['visit']>0) {
    $html .= "<img src='/".PIX_PATH."/action/stats-visitors.png' onclick=\"Kwo.Stats.onExec(this)\" data-action=\"visits\" title='détail des visites pertinentes' class='button' />";
  }
  $html .= "<img src='/".PIX_PATH."/action/export.png' onclick=\"Kwo.Stats.onExec(this)\" data-action=\"export\" title='exporter' class='button' />";
  $html .= "<img src='/".PIX_PATH."/action/events.png' onclick=\"Kwo.Stats.onExec(this)\" data-action=\"events\" title='évènements' class='button' />";
  $html .= "<img src='/".PIX_PATH."/action/flags.png' onclick=\"Kwo.Stats.onExec(this)\" data-action=\"country\" title='pays' class='button' />";
  $html .= "<img src='/".PIX_PATH."/action/stats-path.png' onclick=\"Kwo.Stats.onExec(this)\" data-action=\"path\" title='parcours type' class='button' />";
  $html .= "<img src='/".PIX_PATH."/action/stats-more.png' onclick=\"Kwo.Stats.onExec(this)\" data-action=\"morestats\" title='statistiques avancées' class='button' style='margin-right:0;' />";
  $from = $obj->year.'-'.$obj->month.'-'.$obj->day;
  $html .= "<img src='/".PIX_PATH."/action/stats-print.png' onClick=\"window.location='/back/tracker/fromto?from=".$from."'\" title='imprimer ces stats' class='button' style='display:none;' />";
  $html .= "</div>";

  $html .= "<table class='layout' style='margin-top:4px;'><tr><td class='tdLeft'>\n";
  
  $list_arr['<a href="javascript:void(0)" onclick="Kwo.Stats.onExec(this)" data-action="img">Visites</a>'] = $qty_arr['visit'];
  $list_arr['Visites d’habitués'] = (($qty_arr['visit'] > 0) ? round((($qty_arr['returning']/$qty_arr['visit'])*100),1) : '0').' %';
  $tmp = $type == 'global' ? 'Visiteurs uniques' : 'nouveaux visiteurs';
  $list_arr[$tmp] = (int) $qty_arr['new'];
  //  if ($qty_arr['page']>=1) $tmp = "<a href='/back/tracker/page?$query' target='".$iframe."'>nb pages vues</a>";
  //  else                     $tmp = 'nb pages vues';
  $tmp = 'Pages vues';
  $list_arr[$tmp]   = (int) $qty_arr['page'];
  $list_arr['visites à 1 page (bounce)']   = (($qty_arr['visit'] > 0) ? round((($qty_arr['visit_one']/$qty_arr['visit'])*100),1) : 0)." %";
  if (Extension::exists('performance')) {
    $list_arr['conversions'] = (int) $qty_arr['conversions'];  
  }
  
  $i = 1;
  $html .= "<table class='grid' style='table-layout:fixed'>\n";
  foreach ($list_arr as $k => $v) {
    $html .= "<tr".(($i++%2)?' class="alt"':'')."><td style='width:60%'>$k</td><td>$v</td></tr>\n";
  }
  $html .= "</table>\n";
  
  $html .= "</td><td class='tdRight'>\n";
  
  $referer_arr = AH::decode($obj->referer_arr);
  $html .=  "<div style=\"height:114px; overflow-y:auto;\"><table class='grid'>\n";
  foreach (Tracker::$mediums as $k => $v) {
    $val = ($qty_arr['visit'] > 0) ? round((($referer_arr[$k]['qty'])/$qty_arr['visit'])*100,1) : 0;
    $name = $v;
    if ($k!=Tracker::MEDIUM_DIRECT && $val>=0.01) {
      $name = " <a href=\"javascript:void(0)\" onclick=\"Kwo.Stats.onExec(this)\" data-action=\"referer\" data-type=\"".$k."\">".$name."</a>";
    }
    $html .=  "<tr".(($i++%2)?' class="alt"':'')."><td style='width:50%;'>".$name."</td><td title='".((int)$referer_arr[$k]['qty'])." visites'>".$val." %</td></tr>\n";
  }
  $html .=  "</table></div>\n";
  
  $html .= "</td></tr></table>\n";
  
  //  $html .= "<iframe src='/back/tracker/img?$query' style='width:100%;height:186px;margin-top:0px; border:0px solid black;' scrolling='auto' name='".$iframe."'></iframe>\n";
  $html .= '<div style="width:100%; height:186px; overflow-y:auto; margin-top:0px; border:0px solid black;" class="panel"><center><img src="/back/tracker/graph?'.$query.'"></center></div>'."\n";
  
  return $html;
}
  
}