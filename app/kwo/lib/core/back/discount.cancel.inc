<?php

try {

  $discount = Discount::getInstance((int) $req->id);
  
  $discount->cancel();
  
  $res->sendMessage('ok');

}
catch (Exception $e) { $err->add($e); }
