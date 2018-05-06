<?php

parse_str($params,$a_params);
$a_kwoextension = array();
$rs = $dbo->query2("SELECT IDFG, TITLE FROM formg ORDER BY TITLE");
while ($obj = $rs->fetchObject()) {
  $a_kwoextension[$obj->TITLE] = "[[FORM,".$obj->IDFG."]]";
}

?>

<html>
<head>
<title>Extension</title>

<script>

function placeElem ()
{
  if (document.getElementById('modList').selectedIndex!=0) {
    parent.<?=$a_params['name']?>.insertHtml(document.getElementById('modList')[document.getElementById('modList').selectedIndex].value);
    document.getElementById('modList').selectedIndex = 0;
    parent.hideAllKwoLayer();
  }
}

function hide(event) 
{
  event = window.event || event; 
  element = event.target || event.srcElement;
  element = element.tagName.toLowerCase();
  if (element=='html' || element=='form' || element=='div') parent.hideAllKwoLayer();
}

</script>

<style type='text/css'>
 BODY { text-align:center; }
 *    { font-size:10px; font-family:verdana; color:black; }
 SELECT { width:100%; }
</style>

</head>

<body style='text-align:center' onClick='hide(event)'>

<div style="margin:5px;height: 60px; overflow:auto">
<select name="modList" id="modList"><option value="0">FORMULAIRE</option>
<?php foreach($a_kwoextension as $modName=>$modTag):?>
<option value="<?=$modTag?>"><?=stripslashes($modName)?></option>
<?php endforeach; ?>
</select>
</div>
<p style='margin:0;text-align:center'><a href="#" onClick="placeElem()">Placer</a></p>


</body></html>
