<?php

try {

  //  phpinfo();

  $stemmer = Stemmer::factory(Locale::FR);

  /*
  echo $stemmer->extract('jouer')."\n\n";
  echo $stemmer->extract('adorer')."\n\n";
  echo $stemmer->extract('voler')."\n\n";
  echo $stemmer->extract('tapis')."\n\n";
  echo $stemmer->extract('fameusement')."\n\n";
  echo $stemmer->extract('majestueusement')."\n\n";
  echo $stemmer->extract('parallélisme')."\n\n";
  echo $stemmer->extract('gravement')."\n\n";
  */


  $stemmer->debug = true;
  $stemmer->extract('formation');

  return ;

  $fd = @fopen('lib/search/infos/stems_fr.txt', 'r');
  
  if (!$fd) return ;
 
  while (($line = fgets($fd, 4096)) !== false) {
    $pair = preg_split('/\s+/', $line);
    $from = $pair[0];
    if (strlen($from) <= 2) continue ;
    $to = $pair[1];
    $stem = $stemmer->extract($from);
    if ($to != $stem) {
      echo $from.' : '.$to.' | '.$stem.' '."\n\n";
      $stemmer->debug = true;
      $stemmer->extract($from);
      return ;
    }
    else {
      echo "$from \t\t\t$to [$stem]\n";
    }
  }
  fclose($fd);
  
}
catch (Exception $e) { $err->add($e); }
