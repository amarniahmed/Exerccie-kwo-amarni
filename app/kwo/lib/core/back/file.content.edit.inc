<?php

try {
  $file = new File($req->file_path,'W');
}
catch (Exception $e) {
  $log->error($e);
  $err->add($e);
  exit (0);
}

?>

<div style="margin:10px 10px 10px 10px;">
<form action="/back/core/file.content.store" method="post" id="_form" >
 <input type="hidden" name="file_path" value="<?=$file->getName()?>" />
 <textarea name="content" style="width:100%; height:370px; margin-bottom:10px;"><?=$file->getContent()?></textarea>
 <img src="<?=PIX_URL?>/action/apply.png"
      onclick="$('_form').submit()" class="button" />
 <img src="<?=PIX_URL?>/action/home.png"
      onclick="Kwo.File.view('<?=$file->getName()?>')" class="button" />
</form>
</div>

