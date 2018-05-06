<?php

try {

  $hash = $dbo->asHash('SELECT b.stem_id, count(*) as n'
                       .' FROM '.Engine::TABLE_BUCKET.' as b'
                       .' GROUP BY b.stem_id'
                       .' ORDER BY n DESC'
                       .' LIMIT 200');
  
  $stems = $dbo->asHash('SELECT id, name'
                        .' FROM '.Engine::TABLE_STEM
                        .' WHERE id IN ('.join(',', array_keys($hash)).')');

  $out = array();
  foreach ($hash as $stem_id => $n) {
    $out[$stems[$stem_id]] = $n;
  }

  print_r($out);

}
catch (Exception $e) { $err->add($e); }
