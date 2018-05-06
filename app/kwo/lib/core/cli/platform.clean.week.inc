<?php

try {

  $dbo->exec('UPDATE '.Meta::TABLE.' SET state=0 WHERE state>4096');

}
catch (Exception $e) { $err->add($e); }
