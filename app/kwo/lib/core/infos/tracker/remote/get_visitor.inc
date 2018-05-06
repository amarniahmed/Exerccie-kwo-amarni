<?php

$visitor_key = $req->visitor_key;
if ($req->hasEmptyAttributes('visitor_key')) $visitor_key = 'NOKEY';

$rs = $dbo->query('SELECT visitor_id,visit_id FROM tkr_visitor '.
		  'WHERE (visitor_key=:visitor_key: OR sig=:sig:) AND ins_time>(#time#-10*60)',
		  array(':sig:'=>$req->sig,
			':visitor_key:'=>$visitor_key,
			'#time#'=>time()));
if ($rs === false || $rs->numRows() < 1) return false;

$result = $rs->fetchResult();

$visitor_id = $result(0,0);
$visit_id   = $result(0,1);

if ($visitor_id<1 || $visit_id<1) return false;

$dbo->exec('UPDATE tkr_visitor '.
	  'SET ins_time=:time:, is_used=1 '.
	  'WHERE visitor_id=:visitor_id:',
	  array(':time:'=>time(),
		':visitor_id:'=>$visitor_id));

print($visitor_id.':'.$visit_id);
//$log->trace($visitor_id.':'.$visit_id);