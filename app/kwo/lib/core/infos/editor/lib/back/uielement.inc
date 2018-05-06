<?php

parse_str($req->params,$params_arr);

$res->setSeo('Elements graphiques');
$res->useTemplates('//layer');

?>

<script>
function insertContent(content) {
  var editor = parent.Kwo.registry["<?=$params_arr['name']?>"];
  editor.insertHtml(content);
  parent.hideAllKwoLayer(); 
}
</script>

<?php

$elem_arr['tableau gris'] = "<table class='box gris'> <thead><th>titre</th></thead> <tr><td><div>contenu</div></td></tr></table><br/>";
$elem_arr['tableau bleu'] = "<table class='box bleu'> <thead><th>titre</th></thead> <tr><td><div>contenu</div></td></tr></table><br/>";
$elem_arr['tableau jaune'] = "<table class='box jaune'> <thead><th>titre</th></thead> <tr><td><div>contenu</div></td></tr></table><br/>";
$elem_arr['tableau vert'] = "<table class='box vert'> <thead><th>titre</th></thead> <tr><td><div>contenu</div></td></tr></table><br/>";
$elem_arr['tableau rose'] = "<table class='box rose'> <thead><th>titre</th></thead> <tr><td><div>contenu</div></td></tr></table><br/>";

foreach ($elem_arr as $k => $v) {
  print("<div class='list'>");
  print("<a href=\"javascript:insertContent('".addslashes($v)."')\">".$k."</a>");
  print("</div>\n");
}

