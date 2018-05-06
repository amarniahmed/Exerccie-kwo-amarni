<?php

try {

  if ($req->item) {
    $item = Item::getInstanceByKey($req->item);
  }
  else {
    $item = Item::getInstance(MODEL, (int) $req->id);
  }

  $interaction_type = (int) $req->interaction_type;

  $where = '';
  $bindinds = array();
  if ($req->date) {
    $where = ' AND DATE_FORMAT(i.created_on, "%Y-%m")=:date:';
    $bindings[':date:'] = $req->date;
  }

  $rs = $dbo->query('SELECT i.created_at, i.visit_id,'
                    .     ' INET_NTOA(v.ip) AS ip, i.value'
                    .' FROM '.Interaction::TABLE.' AS i,'
                    .     ' '.Visit::TABLE.' AS v'
                    .' WHERE i.model_id='.$item->getModelId()
                    .  ' AND i.record_id='.$item->getId()
                    .  ' AND i.type='.$interaction_type
                    .  ' AND i.visit_id=v.id'.$where,
                    $bindings); 

  $opts = array('filename' => $item->getModel().'-'.$item->id.'-'.SH::urlify(Interaction::$names[$interaction_type]).'-export');
  $res->sendData($rs, $opts);

}
catch (Exception $e) { $err->add($e); }