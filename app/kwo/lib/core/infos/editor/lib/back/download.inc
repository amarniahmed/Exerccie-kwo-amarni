<?php

$res->reset();
parse_str($req->params,$params_arr);
$path = $req->path;

?>

<html>
<head>
<link href="/<?=APP_PATH?>/<?=CORE?>/css/layer.css" rel="stylesheet" type="text/css" />
</head>

<body>

<?php

if (strlen(DOC_PATH)<strlen($path)) {
  print("<div class='list'>");
  print("<img src='".PIX_URL."/mime/dir.gif' />");
  print("<a href='".$req->getModelId()."?path=".dirname($path)."&params=".urlencode($req->params)."'>..</a>");
  print("</div>\n");
}

$dir = new Dir($path);
$tmp_arr = $dir->getDirs();
$tmp_arr += $dir->getFiles();

foreach ($tmp_arr as $v) {
  print("<div class='list'>");
  if ($v['is_dir']!=1) {
    print("<a href=\"javascript:parent.selectDownloadLink('/".$path."/".$v['name']."')\">");
    print($v['name']);
    print("</a>\n"); 
  }
  else {
    print("<img src='".PIX_URL."/mime/dir.gif'>");
    print(" <a href='".$req->getModelId()."?path=".$path."/".$v['name']."&params=".urlencode($req->params)."'>".$v['name']);
    print("</a>");
  }
  print("</div>\n");
}

