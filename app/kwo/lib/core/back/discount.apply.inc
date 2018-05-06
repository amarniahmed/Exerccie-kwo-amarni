<?php

try {

  $discount = Discount::getInstance((int) $req->id);

  $discount->apply();

  $res->sendMessage('ok');

}
catch (Exception $e) { $err->add($e); }
