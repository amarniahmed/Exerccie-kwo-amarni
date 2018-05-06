<?php 

parse_str($req->params,$params_arr);

$res->setSeo('Style');
$res->useTemplates('//layer');

$styles_arr = array('sub','super');

?>

<script>
function setStyle(s) {
  var editor = parent.Kwo.registry["<?=$params_arr['name']?>"];
  editor.setStyle(s);
  parent.hideAllKwoLayer(); 
}
</script>

<?php

foreach ($styles_arr as $v) {
  print('<div class="list">');
  print(" <a href=\"javascript:setStyle('".$v."');\">".$v."</a>\n"); 
  print('</div>');
}

