<?php

try {

  $db = DashBoard::getInstance();

  $report = Report::getInstance($db);

  $res->write($db);

}
catch (Exception $e) { $err->add($e); }
