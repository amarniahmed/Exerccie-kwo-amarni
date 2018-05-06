<?php

try {

  Item::getInstance(MODEL, $req->id)->setInterests($req->interests);

}
catch (Exception $e) { $err->add($e); }