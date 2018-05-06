<?php

try {

  $dbo->exec('DELETE FROM '.Discount::TABLE_CONDITION
             .    ' WHERE id='.intval($req->condition_id)
             .    ' LIMIT 1');
  
}
catch (Exception $e) { $err->add($e); }
