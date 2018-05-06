<?php

class NH 
{
  public static function asHash($mask, $h) {
    $bits = array();
    for ($i = 0; $i <= 31; $i++) {
      $n = pow(2, $i);
      if ($n > $mask) break ;
      if ($mask & $n) {
        $bits[$n] = $h[$n];
      }
    }
    return $bits;
  }

  public static function asSet($mask) {
    $bits = array();
    if ($mask < 1) return $bits;
    for ($i = 0; $i <= 31; $i++) {
      $n = pow(2, $i);
      if ($n > $mask) break ;
      if ($mask & $n) {
        $bits[] = $n;
      }
    }
    return $bits;
  }

  public static function asFileSize($size, $type='auto') {
    $type = strtolower($type);
    if ($type === 'auto') {
      if ($size > 1048576) $type = 'm';
      elseif ($size > 1024) $type = 'k';
      else $type = 'o';
    }
    if ($type === 'k') $size = number_format($size / 1024, 1, '.', '');
    elseif ($type === 'm') $size = number_format($size / 1048576, 1, '.', '');
    else $size = number_format($size, 0, '', '');
    return $size.' '.strtoupper($type);
  }

  protected static $romans = array('', 
                                   'I', 'II', 'III', 'IV', 'V',
                                   'VI', 'VII', 'VIII', 'IX', 'X');

  protected static $roman = array('M' => 1000, 'CM' => 900, 'D' => 500, 'CD' => 400,
                                  'C' => 100, 'XC' => 90, 'L' => 50, 'XL' => 40,
                                  'X' => 10, 'IX' => 9, 'V' => 5, 'IV' => 4, 'I' => 1);
  
  public static function asRoman($num) {
    $n = (int) $num;
    if ($num <= 10) {
      return self::$romans[$num];
    }
    $result = '';
    foreach (self::$roman as $roman => $value) {
      $matches = (int) ($n / $value);
      $result .= str_repeat($roman, $matches);
      $n = $n % $value;
    }
    return $result;
  }
  
  public static function asBit($n) {
    // 1 -> 1, 2 -> 2, 3 -> 4, 4 -> 8
    //inverse : strlen(decbin($n));
    if (empty($n)) return 0;
    return 1 << ($n - 1);
  }

  public static function asPath($id) {
    $index = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $res = '';
    do {
      $res = $index[$id % 62] . '/' . $res;
      $id = floor($id / 62);
    } while ($id);
    return substr($res, 0, -1);
    /*$index = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $out = '';
    $length = strlen($index);
    $t = floor(log10($id) / log10($length));
    do {
      $b = pow($length, $t);
      $a = floor($id / $b);
      $id -= ($a * $b);
      $out .= $index[$a].'/';
      --$t;
    } while ($t >= 0);
    return trim($out, '/'); */
  }
  

}