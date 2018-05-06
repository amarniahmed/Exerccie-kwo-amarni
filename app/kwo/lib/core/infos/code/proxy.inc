<?php

try {

  if (!$req->hasAttribute('url') || strpos($req->url, '/') === false) {
    throw new Exception('no url');
  }

  $attributes = array();

  if ($req->hasAttribute('item_key')) {
    $item = Item::getInstanceByKey($req->item_key);
    $attributes['model_id'] = $item->getModelId();
    $attributes['record_id'] = $item->getId();
  }

  $attributes['url_id'] = Url::asId($req->url);
  $attributes['visit_id'] = $req->getId();
  $attributes['created_on'] = DH::now();

  if ($attributes['visit_id'] >= 1) {
    $dbo->insert('sys_proxy', $attributes);
  }

  $res->sendRedirect($req->url);

}
catch (Exception $e) { 
  $err->add($e); 
  $res->sendRedirect('/');
}
