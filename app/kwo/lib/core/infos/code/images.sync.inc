<?php

// /photos /photos_reduites /bellefaye_dl/FR/Logos

$server = 's39.kernix.net';
$remote_dir = '.';
$local_dir = '/tmp/bellefaye';
$login = 'bellefaye';
$password = 'Ux5PbNCH';
$out_log = '/tmp/lftp.out';
$err_log = '/tmp/lftp.err';

$cmd = 'lftp -u '.$login.','.$password.' -e "lcd '.$local_dir.'; cd '.$remote_dir.'; mirror --verbose=1; quit" '.$server.' > '.$out_log.' 2> '.$err_log;

exec($cmd);