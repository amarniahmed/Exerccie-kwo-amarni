<?php 

// http://www.colorjack.com/plugin/

parse_str($req->params, $params_arr);

$res->setSeo('Couleur du texte');
$res->useTemplates('//layer');

?>

<div id="colpick"></div>

<script  src="/<?=APP_PATH?>/<?=$req->extension()?>/js/colorpicker.js" type="text/javascript"></script>

<script>
function setCol(c) {
  var editor = parent.Kwo.registry["<?=$params_arr['name']?>"];
  editor.setContent('foreColor', c);
  parent.hideAllKwoLayer(); 
}
function loadDiv() { aff_color_picker('colpick', 'setCol', ''); }
Event.observe(window, "load", loadDiv, false);
</script>