<?php

$rs = $dbo->query('select * from '.Tracker::TABLE_VISITOR.' where counter>0');
foreach ($rs as $hash) {
  print($hash['created_at'].' '.IH::num2ip($hash['ip']).' '.gethostbyaddr(IH::num2ip($hash['ip']))."\n");
}