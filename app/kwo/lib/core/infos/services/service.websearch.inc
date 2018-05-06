<?php

$res->close();

$webSearch = new WebSearch ();
$res->write($webSearch->query($req->query, $req->type));
