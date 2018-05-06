<?php

try {

  $item = Item::getInstance(MODEL, $req->id, 
                            array('metas' => array('interests')));

  $ctx->interests = $item->getInterests(); 
  $ctx->options = Model::asHash('interest');
    
  $res->useXml();

}
catch (Exception $e) { $err->add($e); }
