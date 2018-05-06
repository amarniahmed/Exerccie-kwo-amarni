<?php

$res->reset();
parse_str($req->params, $params_arr);

$action = $_REQUEST['search_action'];
$result = array();

if ($action == "search") {
  $search = implode("%", explode(" ", $_REQUEST['pageName']));
  $rs = $dbo->query("SELECT * FROM page WHERE sname LIKE '%".$search."%' OR lname LIKE '%".$search."%' OR reft LIKE '%".$search."%'");
  if ($rs === false || $rs->numRows() < 1) $rs = $dbo->query("SELECT * FROM page WHERE content LIKE '%".$search."%'");
  while ($row = $rs->getNext()) $result[] = $row;
}
?>

<html>
<head>
<link href="/<?=APP_PATH?>/<?=CORE?>/css/layer.css" rel="stylesheet" type="text/css" />
</head>

<body>
<table cellpadding="4" cellspacing="0" border="0">
<form name="searchpage" action="/back/editor/searchpage" method="post">
<input type="hidden" name="search_action" value="search" />
 <label>Nom de la page</label>
 <input style="width:100px;" class="text" type="text" name="pageName" id="pageName" value="<?=$_REQUEST['pageName']?>" />
 &nbsp;<a href="javascript:document.searchpage.submit();"><img border="0" src="/<?=PIX_PATH?>/action/search.png" /></a>
 <br/><br/>

<?php if ($action == "search"): ?>
<?php if (!empty($result)):?>
<span><?=count($result)?> page(s) trouv&eacute;e(s) :</span><br />

<?php foreach($result as $pagefound):?>
<div class="list">
 <a href="javascript:parent.selectInternLink('<?=$pagefound["ID"]?>', '<?=$pagefound["SNAME"]?>');" title="<?=$pagefound['LNAME']?>"><?=$pagefound["SNAME"]?></a>
</div>
<?php endforeach;?>
<?php else:?>
<em>Aucune page trouv&eacute;e</em><br />
<?php endif;?>
<?php endif;?>

</body>
</html>
