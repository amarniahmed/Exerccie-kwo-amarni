<?php

try {

  $opts = array('filters' => array('user_id' => $user->getId()),
                'metas' => array('created_at'),
                'reverse' => true);

  $favorites = Favorite::collect($opts);

  $errors = 0;
  foreach ($favorites as $favorite) {
    try {
      $favorite->getItem();
      if (!$favorite->isOnline()) {
        throw new Exception('item is offline');
      }
    }
    catch (Exception $e) {
      $error++;
      $favorite->destroy();
    }
  }

  if ($errors > 0) {
    $favorites->reset();
  }

  $ctx->favorites = $favorites;

  $res->setMenu('favorites');
  $res->setTitle(l('account.favorites'));

}
catch (Exception $e) { $err->add($e); }
