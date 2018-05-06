<?php

try {

  $code = 'shop.return.motives';

  $user = $req->getUser();

  if (Model::exists('tree') && Item::exists('tree', $code)) {
    $ctx->tree = Tree::getInstance($code);
  }

  $res->useTemplate();

}
catch (Exception $e) { $err->add($e); }
