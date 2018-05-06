<?php

try {

  $conversion = Item::getInstanceByKey($req->item); 

  $conversion->setWinner($req->visit_id);

}
catch (Exception $e) { $err->add($e); }
