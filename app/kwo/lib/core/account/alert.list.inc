<?php

try {

  $opts = array('filters' => array('user_id' => $user->getId(),
                                   '|>|status' => 0/*,
                                   'model_id' => Model::id('keyword')*/),
                'offset' => (int) $req->offset);

  $alerts = Alert::collect($opts);

  $ctx->alert_id = (int) $req->alert_id;
  $ctx->alerts = $alerts;

  $res->setMenu('alerts');
  $res->setTitle(l('account.alert.title'));

}
catch (Exception $e) { $err->add($e); }
