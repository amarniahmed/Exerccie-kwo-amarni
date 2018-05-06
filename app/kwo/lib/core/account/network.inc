<?php

try {

  $ctx->friends = Network::friends($user, array('collection' => true));

  $res->setMenu('network');
  $res->setTitle(l('account.network'));

}
catch (Exception $e) { $err->add($e); }
