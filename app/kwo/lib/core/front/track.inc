<?php

try {
  
//  d($req->getHeaders());

//  $req->debug();

  if (P('app.tracker') === false) return ;

  if (!$req->isXhr()) {
    return ;
  }



  $tracker = Tracker::getInstance(); 

  if (P('app.load') > 0) return ;

  $visit_id = $req->getVisitId(); 
  if ($visit_id < 0) return ;

  if ($req->params[0] != '{') {
    S('log')->error($req->params);
    throw new Exception('invalid params');
  }

  $h = json_decode($req->params, true);

  if (isset($h['events']) && is_array($h['events'])) {
    foreach ($h['events'] as $event) {
      AH::trim($event);
      if (empty($event) || empty($event[0]) || empty($event[1]) || intval($event[2]) < 2) continue ;
      $seal = (int) array_pop($event);
      if ($seal != AH::seal($event)) {
        S('log')->debug($event);
        continue ;
      }
      $req->track($event[0], $event[1]); 
    }
  }

  if (isset($h['interactions']) && is_array($h['interactions'])) {
    $opts = array();
    foreach ($h['interactions'] as $interaction) {
      AH::trim($interaction);
      if (empty($interaction) || $interaction[0] < 1 || $interaction[1] < 1 || $interaction[2] < 1) continue ;
      $seal = (int) array_pop($interaction);
      if ($seal != AH::seal($interaction)) {
        S('log')->debug($interaction);
        continue ;
      }
      $opts['value'] = (int) $interaction[3];
      $opts['visit_id'] = $visit_id;
      Interaction::track(array($interaction[0], $interaction[1]), $interaction[2], $opts);
    }
  }

  Item::log();

  $ctx->visit_id = $req->getVisitId(); 
  $ctx->visitor_id = $req->getVisitorId();
  $ctx->is_new_visit = 0;
  $ctx->is_new_visitor = 0;

  if ($req->isNewVisit()) {
    $ctx->is_new_visit = 1;
    if ($req->isNewVisitor()) {
      $ctx->is_new_visitor = 1;
    }
    if (Model::exists('popup')) {
      $ctx->popup_id = Popup::findId($req);
    }
  }

//  if (S('app')->getName() == DEVEL) $ctx->popup_id = Popup::findId($req);
 
  S('app')->hook('onNewVisit');

  $res->disableCache();

}
catch (Exception $e) {
  $log->error($e);
}