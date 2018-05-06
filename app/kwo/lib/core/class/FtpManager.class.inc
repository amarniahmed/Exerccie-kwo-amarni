<?php

// /photos /photos_reduites /bellefaye_dl/FR/Logos                                                                                  

/*$server = 's39.kernix.net';                                                                                                     
$remote_dir = '.';                                                                                                                  
$local_dir = '/var/web/photos';                                                                                                     
$login = 'bellefaye';                                                                                                               
$password = 'Ux5PbNCH';                                                                                                             
$out_log = '/tmp/lftp.images.out';                                                                                                  
$err_log = '/tmp/lftp.images.err';                                                                                                  
$ftp_cmd = 'mirror --verbose=1 -e';*/

$server = 'xxx.xxx.xxx.xxx';
$remote_dir = '.';
$local_dir = '/var/web/photos';
$login = '';
$password = '';
$out_log = '/tmp/lftp.images.out';
$err_log = '/tmp/lftp.images.err';
$ftp_cmd = 'mirror  --parallel=4 --verbose=1';

foreach (array('photos', 'photos_reduites', 'FR/Logos') as $dir) {                
  //foreach (array('/testmirror', '/pubtemp') as $dir) {                                                                              
  $ftp_cmd2 = $ftp_cmd . ' ' . $dir;
  $cmd = 'lftp -u '.$login.','.$password.' -e "lcd '.$local_dir.'; cd '.$remote_dir.'; '.$ftp_cmd2.'; quit" '.$server.' > '.$out_log.' 2> '.$err_log;   
  error_log($cmd);
  exec($cmd);

}