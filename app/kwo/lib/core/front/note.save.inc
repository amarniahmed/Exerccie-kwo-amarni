<?php

if (!$req->isXhr()) {
  $log->trace('bad ping');
  return ;
}

try { 

  $item = Item::getInstanceByKey($req->item);
  
  $value = $req->note < 1 ? 1 : $req->note % (Valuation::RATE_MAX + 1);
  
  $flag = $item->track(Interaction::TYPE_EVALUATE, array('value' => $value)); 
  
  if ($flag) {
    $h = $dbo->fetchHash('SELECT COUNT(*) AS c, SUM(value) AS s'
                         .' FROM '.Interaction::TABLE
                         .' WHERE model_id='.$item->getModelId()
                         .  ' AND record_id='.$item->getId()
                         .  ' AND type='.Interaction::TYPE_EVALUATE
                         .' GROUP BY model_id, record_id'); 
    if (count($h) >= 1 && $h['c'] > 0) {
      $h['s'] = (int) (round($h['s'] / $h['c']) % 6);
      $item->setMeta(Meta::FIELD_RATING, $h['s']);
    }
  }
  
}
catch (Exception $e) { $log->error($e); }

$res->sendMessage(l('thank_you'));