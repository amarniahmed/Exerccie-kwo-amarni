<?php

try {

  $user = $req->getUser();

  $item = Item::getInstanceByKey($req->item);

  $opts = array('filters' => array('model_id' => $item->getModelId()),
                'order' => array('position'));
  
  $criterions = Criterion::collect($opts);

  $ctx->criterions = $criterions;

  $template = false ? 'valuation.radios' : 'valuation.stars';

  $res->useTemplate($template);

}
catch (Exception $e) { $err->add($e); }