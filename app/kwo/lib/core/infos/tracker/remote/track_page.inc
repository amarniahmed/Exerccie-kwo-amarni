<?php

$ins_arr['visit_id'] = $req->visit_id;
$ins_arr['title']    = $req->title;
$ins_arr['chapter']  = $req->chapter;
$ins_arr['lead']     = $req->lead;

$dbo->insert('tkr_tmp', $ins_arr);