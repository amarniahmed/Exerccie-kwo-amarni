<?php

try {

  if ($req->id > 0) {
    $item = Item::getInstance(MODEL, (int) $req->id);
  }
  else {
    $item = Item::getInstanceByKey($req->item);
  }
  
  if ($item instanceof Purchasable) {
    $rs = $dbo->query('SELECT p.order_id, o.total_amount AS order_amount, o.paid_at, o.user_id, u.email, p.price, p.tax, p.quantity'
                      .' FROM '.T('purchase').' AS p,'
                      .     ' '.T('order').' AS o,'
                      .     ' '.T('user').' AS u'
                      .' WHERE p.model_id='.$item->getModelId()
                      .  ' AND p.record_id='.$item->getId()
                      .  ' AND p.order_id=o.id'
                      .  ' AND o.status>='.Order::STATUS_PAID
                      .  ' AND o.user_id=u.id'
                      .' ORDER BY o.id DESC');
  }
  else {
    $model = $req->purchase_model;
    $foreign_key = Model::name($item).'_id';
    $rs = $dbo->query('SELECT p.order_id, o.total_amount AS order_amount, o.paid_at, o.user_id,'
                      .     ' u.email, i.id AS '.$model.'_id, p.name, p.code, p.price, p.tax, p.quantity'
                      .' FROM '.T('purchase').' AS p,'
                      .     ' '.T('order').' AS o,'
                      .     ' '.T('user').' AS u,'
                      .     ' '.T($model).' AS i'
                      .' WHERE p.model_id='.Model::id($model)
                      .  ' AND p.record_id=i.id'
                      .  ' AND i.'.$foreign_key.'='.$item->getId()
                      .  ' AND p.order_id=o.id'
                      .  ' AND o.status>='.Order::STATUS_PAID
                      .  ' AND o.user_id=u.id'
                      .' ORDER BY o.id DESC');
  }
    
  $filename = $item->getModel().'-'.$item->getId().'-orders';
  $res->sendData($rs, array('filename' => $filename));

}
catch (Exception $e) { $err->add($e); }
