<?php

Platform::import('Tracker');

$url_arr = parse_url($req->request_uri);
$req_arr = array();
parse_str($url_arr['query2'],$req_arr);

$referer_arr = Tracker::getReferer($req->http_referer,$req_arr);

$ins_arr['visitor_id']   = $req->visitor_id;
$ins_arr['number']    = $req->number;
$ins_arr['medium'] = $referer_arr['grp'];
$ins_arr['source'] = $referer_arr['key'];
$ins_arr['term']  = $referer_arr['tag'];
$ins_arr['remote_ip']    = $req->remote_ip;
$ins_arr['remote_host']  = IH::ip2host($req->remote_ip);
$ins_arr['user_agent']   = IH::ua2browser($req->user_agent);
$ins_arr['country_code'] = IH::ip2country($req->remote_ip);
$ins_arr['prev_time']    = $req->prev_time;
$ins_arr['prev_date']    = date(DH::FORMAT_DB,$req->prev_time);
$ins_arr['first_date']   = date(DH::FORMAT_DB,$req->first_time);
    
$visit_id = $dbo->insert('tkr_visit',$ins_arr);

print($visit_id);

if ($visit_id<1) {
  $log->error('can not insert visit for visitor : '.$req->visitor_id);
  return false;
}

$dbo->exec('UPDATE tkr_visitor '.
	  'SET visit_id=:visit_id: '.
	  'WHERE visitor_id=:visitor_id:',
	  array(':visit_id:'=>$visit_id,
		':visitor_id:'=>$req->visitor_id));
