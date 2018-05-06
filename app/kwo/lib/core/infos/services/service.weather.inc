<?php

$res->close();

$weather = new Weather ();

if ($req->type == 'search') {
  $res->write($weather->search($req->query));
}
