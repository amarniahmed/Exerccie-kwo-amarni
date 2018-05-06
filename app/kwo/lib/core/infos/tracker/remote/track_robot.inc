<?php

$ins_arr['uri']   = $req->request_uri;
$ins_arr['robot'] = $req->http_user_agent;

$dbo->insert('tkr_robot',$ins_arr);