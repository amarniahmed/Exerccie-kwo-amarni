<?php

parse_str($req->params,$params_arr);

$res->setSeo('Fichier');
$res->useTemplates('//layer');

if (!empty($req->path)) $path = $req->path;
elseif (!empty($params_arr['path'])) $path = $params_arr['path'];
elseif (!empty($params_arr['file_path'])) $path = dirname($params_arr['file_path']);

try { $dir = new Dir($path); }
catch (Exception $e) { return $err->add($e); }

$path = $dir->getPath();

if ($params_arr['filter']=='img') $files_arr = $dir->getFiles(Dir::FILTER_IMG);
else $files_arr = $dir->getFiles();

$tmp_arr  = AH::concat($dir->getDirs(), $files_arr);

foreach ($tmp_arr as $file) {
  print('<div class="list">');
  if ($file['is_dir']==1) {
    print('<img src="/'.PIX_PATH.'/mime/dir.gif" />');
    print("<a href='".$req->getUrl()."?path=".$file['path']."&params=".urlencode($req->params)."'>".$file['name']);
    print("</a>");
  }
  else {
    print("<a href=\"javascript:placeElem('".$params_arr['input_id']."','/".$path."/".$file['name']."')\">");
    if ($params_arr['filter']=='img') print("<img src='/".$path."/".$file['name']."' class='thumb' title='".$file['name']."' />");
    else print($file['name']);
    print("</a>"); 
  }
  print("</div>\n");
}
