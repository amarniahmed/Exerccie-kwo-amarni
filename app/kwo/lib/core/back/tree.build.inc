<?php

try {

  $tree = Tree::getInstance($req->id);
  $tree->build();

  $res->sendMessage(c(l('construction effectuée')));

}
catch (Exception $e) { $err->add($e); }