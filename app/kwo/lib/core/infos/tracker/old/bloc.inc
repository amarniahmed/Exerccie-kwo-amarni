<?php

$year  = (int) $req->year;
$month = (int) $req->month;
$day   = (int) $req->day;

echo $this->renderBloc($year,$month,$day);
