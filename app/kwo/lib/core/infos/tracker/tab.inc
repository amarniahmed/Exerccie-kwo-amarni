<?php

$rs = $dbo->query("SELECT * FROM {$table['tkr_cache']} WHERE year = '$year' AND month = '$month' AND day = '$day'");
if ($rs->numRows() < 1) return 0;
$obj = $rs->fetchObject();

$l_nbres = 20;

if ($p_sa=='se') {
  $l_nbres = 50;
  include("$g_codepath/$p_m/tab-se.inc");
}
elseif ($p_sa=='links') {
  $l_nbres = 20;
  include("$g_codepath/$p_m/tab-links.inc"); 
}
elseif ($p_sa=='morestats') include("$g_codepath/$p_m/tab-morestats.inc");
elseif ($p_sa=='country') include("$g_codepath/$p_m/tab-country.inc");
elseif ($p_sa=='path') {
  $l_nbres = 5;
  include("$g_codepath/$p_m/tab-path.inc");
}
elseif ($p_sa=='page') {
  $l_nbres = 20;
  include("$g_codepath/$p_m/tab-page.inc"); 
}
elseif ($p_sa=='img') {
  //  print("<center><img src='admin.php?p_m=$p_m&p_a=img&year=$year&month=$month&day=$day&p_flag=$p_flag'></center>");
  print('<center><img src="/'.PIX_PATH.'/empty.gif" id="'.$py.'-'.$p_m.'-'.$p_d.'"/></center>'."\n");
  print("<script>var d = new Array (); d[0] = new Image; d[0].src='admin.php?p_m=$p_m&p_a=img&year=$year&month=$month&day=$day&p_flag=$p_flag';getId('$py-$p_m-$p_d').src=d[0].src;</script>\n");
}
