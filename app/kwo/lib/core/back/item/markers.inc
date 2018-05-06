<?php

try {

  $table = $dbo->getTable(T(MODEL));
  if ($table->hasField('latitude')) {
    $items = Item::collect(MODEL, $opts);
  }
  else {
    $n = $dbo->fetchValue('SELECT id FROM '.T('geolocation')
                          .' WHERE model_id='.Model::id(MODEL)
                          .' LIMIT 1');
    $rs = $dbo->query('SELECT m.*, g.latitude, g.longitude'
                      .' FROM '.T(MODEL).' AS m,'
                      .     ' '.T('geolocation').' AS g'
                      .' WHERE g.model_id='.Model::id(MODEL)
                      .  ' AND g.record_id=m.id');
    $items = $rs->asCollection(MODEL);
  }
  
  $markers = array();

  foreach ($items as $item) {
    if ($item->latitude == 0 && $item->longitude == 0) continue ;
    $color = ''; // _yellow, _green
    $h = array('lat' => $item->latitude,
               'lng' => $item->longitude,
               'name' => $item->getName(),
               'id' => $item->getId(),
               'color' => '',
               'code' => $color);
    $markers[] = $h;
  }

  if (!$req->isXhr()) {
    $content  = 'var markers = ';
    $content .= json_encode($markers);
    $content .= "\n".';';
    $res->resetTemplates();
    $res->write($content);
    return ;
  }

  $ctx->markers = $markers;
  
}
catch (Exception $e) { $err->add($e); }
