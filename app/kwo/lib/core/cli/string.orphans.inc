<?php

try {

  $strings = $dbo->asHash('SELECT name, id FROM '.T('string').' WHERE (flags & 1) = 0 AND app="" ORDER BY name');
  foreach ($strings as &$value) $value = array();

  $files = array();
  $cmd = '/usr/bin/find . -type f \( -name "*.inc" -o -name "*.psp" \)';
  exec($cmd, $files);

  foreach ($files as $file) {
    if (preg_match('#/(tmp|infos|etc|locales)/#', $file)) continue ;
    $content = file_get_contents($file); 
    foreach ($strings as $string => &$value) {
      if (count($value) >= 1) continue ;
      if (strpos($content, "l('".$string."'") === false) continue ;
      $value[] = $file;
    }
  }

  print_r($strings);
  return ;

  foreach ($strings as $string => $files) {
    if (!empty($files)) continue ;
    echo $string."\n";
    $item = String::getInstance(array('name' => $string));
    $item->destroy();
  }

}
catch (Exception $e) { $err->add($e); }
