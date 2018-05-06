<?php

class Timezone extends ActiveRecord {


  public static function asHash($locale) {
    $out = array();
    $rs = S('dbo')->query('SELECT * FROM '.T('timezone').' WHERE status > 0 ORDER BY code, utc');
    foreach ($rs as $row) {
      $out[$row['id']] = $row['code'].' UTC'.$row['utc'];
    }
    //    asort($out);
    return $out;
  }


}