<?php

try {

  $db = DashBoard::getInstance($req->period);
  $res->write($db);

}
catch (Exception $e) { $err->add($e); }
