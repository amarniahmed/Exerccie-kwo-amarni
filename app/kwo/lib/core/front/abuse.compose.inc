<?php

try {

  //$filters = array('locale' => $res->getLocale());
  $opts = array('filters' => $filters,
                'order' => array('position' => 'asc'),
                'locale' => $res->getLocale());
  $motives = Motive::collect($opts);

  $ctx->motives = $motives->asHash('id', 'label');
  $ctx->item = $req->item;  

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
