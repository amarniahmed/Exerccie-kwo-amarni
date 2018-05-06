<?php

$ins_arr['sig']         = $req->sig;
$ins_arr['visitor_key'] = $req->visitor_key;
$ins_arr['ins_time']    = time();
    
$visitor_id = $dbo->insert('tkr_visitor',$ins_arr);

print($visitor_id);