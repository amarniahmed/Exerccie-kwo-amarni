<?php

parse_str($req->params,$params_arr);

$res->setSeo('Images');
$res->useTemplates('//layer');

if (empty($req->path)) {
  if (!empty($params_arr['path'])) $path = $params_arr['path'];
  elseif (!empty($params_arr['file'])) $path = dirname($params_arr['file']);
}
else $path = $req->path;

try {
  $dir = new Dir($path);
  $path = $dir->getPath();
}
catch (Exception $e) { return $err->add($e); }

$tmp_arr  = AH::concat($dir->getDirs(),$dir->getFiles(Dir::FILTER_IMG));

foreach ($tmp_arr as $v) {
  print('<div class="list">'."\n");
  if ($v['is_dir']!=1) {
    print("<a href=\"javascript:placeElem('/".$path."/".$v['name']."')\">");
    print("<img src='/".$path."/".$v['name']."' class='thumb' title='".$v['name']."' />");
    print($v['name']);
    print("</a>\n"); 
  }
  else {
    print('<img src="/res/pix/mime/dir.gif" />');
    print(' <a href="'.$req->getModelId().'?path='.$v['path'].'&params='.urlencode($req->params).'">'.$v['name'].'</a>');
  }
  print('</div>'."\n");
}

?>

<select id="align" style="width:auto; margin-bottom:2px;">
 <option value="top">Texte haut</option>
 <option value="absmiddle">Texte milieu</option>
 <option value="absbottom">Texte bas</option>
 <option value="right">Droite</option>
 <option value="left">Gauche</option>
</select> -  
<input type="checkbox" id="border" style="margin:0; padding:0;" /> bord -
<input type="checkbox" id="padding" style="margin:0; padding:0;" /> espace


<script>
function placeElem (src) {
  align   = 'align='+$F('align'); 
  border  = ($('border').checked==true)?'border:1px solid black;':''; 
  padding = ($('padding').checked==true)?'margin:8px;':''; 
  html = '<img src="'+src+'" '+align+' style="'+border+padding+'" />';
  var editor = parent.Kwo.registry["<?=$params_arr['name']?>"];
  editor.insertHtml(html, src);
  parent.hideAllKwoLayer();
}
</script>


