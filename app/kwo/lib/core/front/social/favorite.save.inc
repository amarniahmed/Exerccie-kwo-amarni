<?php

try { 

  $user = $req->getUser();

  $item = Item::getInstanceByKey($req->item);

  $opts = array('comment' => $req->comment); 

  Favorite::save($item, $user, $opts);

  $snippet = Snippet::get('favorite.confirm');
  
  $req->track();  

  $res->sendMessage($snippet->content);

}
catch (Exception $e) { $err->add($e); }
