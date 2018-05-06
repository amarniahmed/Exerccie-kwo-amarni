<?php

try { 
  
  $url = Notification::$services[$req->service_id]['url'];
  
  if (strlen($url) < 8) {
    throw new Exception('invalid service id ('.$req->service_id.')');
  }
  
  $item = Item::getInstanceByKey($req->item);  
  
  $attributes = array('model_id' => $item->getModelId(),
                      'record_id' => $item->getId(),
                      'type' => Notification::TYPE_SITE,
                      'service_id' => $req->service_id); 
  $notification = Notification::getInstance();
  $notification->commit($attributes);
  $notification->setVisit($req);
  $item->incrementMeta(Meta::FIELD_SHARES); 

  if (method_exists($item, 'asShareHash')) {
    $bindings = $item->asShareHash($res->getLocale());
  }
  else {
    $target_url = $item->asUrl();
    $target_url = H::url($target_url);
    $taget_name = $item->getName($res->getLocale());
    $bindings = array('url' => $target_url,
                      'title' => $taget_name);
  }
  
  if (!isset($bindings['srcurl'])) {
    $bindings['srcurl'] = H::url();
  }
  if (!isset($bindings['srctitle'])) {
    $bindings['srctitle'] = P('app.name');
  }
  $bindings['snippet'] = $bindings['snippet'] ? $bindings['snippet'] : '';
  $bindings['tags'] = $bindings['tags'] ? $bindings['tags'] : ''; 
  
  foreach ($bindings as &$value) {
    $value = is_array($value) ? join(', ', $value) : $value;
    $value = urlencode($value);
  }

  $url = SH::bind($url, $bindings);
  
  $req->track();
  
  $res->sendRedirect($url);

}
catch (Exception $e) { 
  $log->error($e); 
  $res->sendRedirect('/');
}

