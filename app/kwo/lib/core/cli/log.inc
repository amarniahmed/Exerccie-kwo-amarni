<?php

//http://rpm.pbone.net/index.php3/stat/4/idpl/17586122/dir/redhat_el_5/com/multitail-5.2.9-1.el5.rf.i386.rpm.html

if ($req->full == 'yes') {
  if (false && is_executable('/usr/bin/multitail')) {
    $cmd = 'multitail '.Logger::ERR_FILE.' '.Logger::URL_FILE;
  }
  else {
    $cmd = 'tail -n 200 -f '.Logger::ERR_FILE.' '.Logger::URL_FILE;
  }
}
else {
  $cmd = 'tail -n 200 -f '.Logger::ERR_FILE;
}

/*if ($req->re) {
  $cmd .= ' | grep '.$req->re;
}

if ($req->hl) {
  $cmd .= ' | perl -pe "s/'.$req->hl.'/\e[1;31;43m$&\e[0m/g"';
}*/

system($cmd);
//passthru($cmd);