<?php

try { 

  $item = Item::getInstanceByKey($req->item);

  $rows = Item::asRelationRows($item, $req->models, $req->table);

  $res->write($rows);

}
catch (Exception $e) { $err->add($e); }
