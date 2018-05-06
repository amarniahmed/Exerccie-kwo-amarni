<?php

try {
  $file = new Img($req->file_path);
}
catch (Exception $e) {
  $log->error($e);
  $err->add($e);
  exit (0);
}

File::show_file_path($file->getPath());

?>

<div style="overflow-x:auto; overflow-y:auto;width:680px; height:410px; margin:12px auto 0 auto; text-align:center;">
<form action="/back/file/img">
 <input type="hidden" name="step" value="<?=($req->step+1)?>" id="step" />
 <input type="hidden" name="x1" value="<?=$req->x?>" />
 <input type="hidden" name="y1" value="<?=$req->y?>" />
 <input type="hidden" name="file_path" value="<?=$req->file_path?>" />
 <input type="image" src="/<?=$file->getName()?>?time=<?=time()?>" />
</form>
</div>

<script>

var _file_path = "<?=$file->getName()?>";
var x1,y1,x2,y2;

function nextStep() {
  if ($F('step')==1) alert('Veuillez selectionner le 1er point');
  else if ($F('step')==2) alert('Veuillez selectionner le 2ieme point');
  else if ($F('step')==3) {
    if (confirm('Confirmez vous la sélection cette zone ?')) {
      x1 = <?=$req->x1?>;
      y1 = <?=$req->y1?>;
      x2 = <?=$req->x?>;
      y2 = <?=$req->y?>;
      file.crop();
    }
    else file.view('<?=$file->getName()?>');
  }
}

Event.observe(window, "load", nextStep, false);

</script>