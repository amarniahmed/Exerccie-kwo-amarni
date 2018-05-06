<?php

final class AH
{
  private static $crc = array();

  public static function crc($input) {
    $crc = array();
    array_walk_recursive($input, array(self, 'getCrcCallback'));
//    print_r(self::$crc);
    sort(self::$crc);
    $str = join('', self::$crc);
//    print("$str\n");
    self::$crc = array();
    return SH::crc32($str);
  }

  protected static function getCrcCallback($value, $key) {
    $str = '';
    if (!empty($value) && is_scalar($value)) {
      if (!empty($key) && !is_numeric($key)) {
        $str .= $key.'=';
      }
      $str .= $value;
    }
    if (!empty($str)) {
      self::$crc[] = $str.'|';
    }
  }

  public static function crush(&$input) {
    if (!is_array($input)) return ;
    foreach ($input as $key => &$value) {
      $value = is_array($value) ? serialize($value) : $value;
    }
  }

  public static function duplicate($hash) {
    //    return array_diff_key($hash, array());
    $out = $hash + array();
    return $out;
  }

  public static function merge($old_arr, $new_arr) {
    $old_arr = (array) $old_arr;
    if (!is_array($new_arr)) $new_arr = @unserialize($new_arr);
    if (!is_array($new_arr)) return $old_arr;
    foreach ($new_arr as $k0 => $v0) {
      if (is_array($v0)) {
        if (!is_array($old_arr[$k0])) $old_arr[$k0] = array();
        foreach ($v0 as $k1 => $v1) {
          if (is_array($v1)) {
            if (!is_array($old_arr[$k0][$k1])) $old_arr[$k0][$k1] = array();
            foreach ($v1 as $k2 => $v2) $old_arr[$k0][$k1][$k2] += (int) $v2;
          }
          else $old_arr[$k0][$k1] += (int) $v1;
        }
      }
      else $old_arr[$k0] += $v0;
    }
    return $old_arr;
  }

  public static function trim(&$input) {
    if (!is_array($input)) return ;
    foreach ($input as $key => $value) {
      if (is_array($value) || is_object($value)) continue ;
      $input[$key] = trim($value);
    }
    return $input;
  }

  public static function seal($input) {
    if (!is_array($input)) return 10;
    if (empty($input)) return 100;
    $n = 0;
    foreach ($input as $value) {
      $value = (string) $value;
      $n += ord($value[0]) + strlen($value);
    }
    $n *= (128 * ord($value[0]));
    return $n;
  }

  public static function asAttributes($in, $protect=false) {
    if (!is_array($in) || empty($in)) return '';

    $out = array();

    if (empty($in['class'])) unset($in['class']);
    if (empty($in['data']))  unset($in['data']);
    if (empty($in['on']))    unset($in['on']);
    if (empty($in['style'])) unset($in['style']);

    if (is_array($in['on'])) {
      foreach ($in['on'] as $event => $value) {
        $in['on'.$event] = $value;
      }
      unset($in['on']);
    }

    if (is_array($in['data'])) {
      foreach ($in['data'] as $key => $value) {
        if (is_array($value) && ($key === 'values' || $key === 'args')) {
          $value = H::asDataValues($value);
        }
        $in['data-'.$key] = $value;
      }
      unset($in['data']);
    }

    foreach ($in as $key => $value) {
      if ($key === 'style') {
        if (is_array($value)) {
          $tmp = key($value);
          if (is_numeric($tmp)) {
            $value = join('; ', $value);
          }
          else {
            foreach ($value as $k => &$v) {
              $v = $k.': '.$v.';';
            }
            $value = join(' ', $value);
          }
        }
      }
      else if (is_array($value)) {
        $value = join(' ', $value);
      }
      if ($protect === true) {
        $value = h($value);
      }
      $out[] = $key.'="'.$value.'"';
    }

    return join(' ', $out);
  }

  public static function asTag($tag, $attrs) {
    $tag = strtolower($tag);
    $end = '>';
    $void = array('area', 'base', 'br', 'col', 'command', 'embed', 'hr', 'img',
                  'input', 'keygen', 'link', 'meta', 'param', 'source', 
                  'track', 'wbr');
    if (in_array($tag, $void)) $end = '/>';
    return '<'.$tag.' '.self::asAttributes($attrs).' '.$end;
  }

  public static function back($arr,$id) {
    $arr = (array) $arr;
    if (count($arr)<=1) return $arr;
    $id_arr = array();
    $i = 0;
    $pos = null;
    foreach ($arr as $k => $v) {
      $id_arr[] = $k;
      if ($k==$id) $pos = $i;
      $i++;
    }
    if ($pos === null || $pos===0) return $arr;
    $tmp = $id_arr[$pos-1];
    $id_arr[$pos-1] = $id_arr[$pos];
    $id_arr[$pos] = $tmp;
    foreach ($id_arr as $v) $ret_arr[$v] = $arr[$v];
    return $ret_arr;
  }

  public static function forward($arr,$id) {
    $arr = (array) $arr;
    if (($n=count($arr))<=1) return $arr;
    $id_arr = array();
    $i = 0;
    $pos = null;
    foreach ($arr as $k => $v) {
      $id_arr[] = $k;
      if ($k==$id) $pos = $i;
      $i++;
    }
    if ($pos===null || $pos===($n-1)) return $arr;
    $tmp = $id_arr[$pos+1];
    $id_arr[$pos+1] = $id_arr[$pos];
    $id_arr[$pos] = $tmp;
    foreach ($id_arr as $v) $ret_arr[$v] = $arr[$v];
    return $ret_arr;
  }

  public static function moveCell($in_arr, $index, $way='right', $cycle=true) {
    $in_arr = (array) $in_arr;
    if (($n=count($in_arr)) <= 1) return $in_arr;

    $keys_arr = array_keys ($in_arr);
    $pos = array_search ($index, $keys_arr);
    if ($pos===false) return $in_arr;

    $from = $pos;
    if ($pos===($n-1)) {
      $to = 0;
    }
    else {
      $to = $pos + 1;
    }

    $tmp = $keys_arr[$to];
    $keys_arr[$to] = $keys_arr[$from];
    $keys_arr[$from] = $tmp;
    foreach ($keys_arr as $key) {
      $out_arr[$key] = $in_arr[$key];
    }

    return $out_arr;
  }

  public static function hasKey($input_arr, $keys_arr) {
    $input_arr = (array) $input_arr;
    $keys_arr = (array) $keys_arr;
    foreach ($keys_arr as $key) {
      if (in_array($key, $input_arr)) return true;
    }
    return false;
  }

  public static function decode($str) {
    $str = trim($str);
    if (empty($str)) return array();
    $ret = (array) @unserialize($str);
    return $ret;
  }

  public static function encode($arr) {
    if (!is_array($arr)) return serialize(array());
    return serialize($arr);
  }

  public static function concat() {
    $result = array();
    foreach (func_get_args() as $arr) {
      $arr = (array) $arr;
      foreach ($arr as $v) $result[] = $v;
    }
    return $result;
  }

  public static function asSql($input, $protect=true) {
    if (empty($input)) $output = '1';
    $pairs = array();
    foreach ($input as $key => $value) {
      if ($protect === true) {
        $value = S('dbo')->protect($value);
      }
      $pairs[] = '`'.$key.'` = '.$value;
    }
    return join(' AND ', $pairs);
  }

  public static function toSet($hash, $enclose=false) {
    if (!is_array($hash)) {
      $hash = SH::asSet($hash);
    }
    return $enclose === false ? join(',', $hash) : "'".join("','", $hash)."'";
  }

  public static function match($arr1, $arr2) {
    $arr1 = (array) $arr1;
    $arr2 = (array) $arr2;
    foreach ($arr1 as $v) {
      if (!in_array($v, $arr2)) return $v;
    }
    return true;
  }

  public static function insert(&$hash, $item, $index) {
    if ($index < 1) {
      array_unshift($hash, $item);
      return ;
    }
    if ($index >= count($hash)) {
      array_push($hash, $item);
      return ;
    }
    $hash = array_merge(array_slice($hash, 0, $index),
                        array($item),
                        array_slice($hash, $index));
  }

  public static function asSet($input, $sep=',', $replace_space=false) {
    if (!is_array($input)) {
      if ($replace_space === true) {
        $input = str_replace(array(' '), array(','), $input);
      }
      $input = SH::asSet($input);
    }
    return join($sep, $input);
  }

  public static function asXml($h, $opts=null) {
    $opts = (array) $opts;
    $tag = $opts['tag'] ?: 'xml';
    $attrs = (array) $opts['attrs'];
    $xml = self::asXmlValue($h);
    foreach ($attrs as $key => &$value) {
      $value = $key.'="'.$value.'"';
    }
    $xml = trim($xml);
    $open_tag = '<'.$tag;
    if (!empty($attrs)) {
      $open_tag .= ' '.join(' ', $attrs);
    }
    $open_tag .= '>'."\n";
    return $open_tag . $xml . "\n" . '</'.$tag.'>';
  }

  public static function isNumeric($h) {
    foreach ($h as $key => $value) {
      if (!is_int($key)) return false;
    }
    return true;
  }

  private static function asXmlValue($h) {
    $xml = '';
    foreach ($h as $key => $value) {
      $xml .= "\n".'<'.$key;
      if (is_array($value)) {
        if (self::isNumeric($value)) {
          $xml .= ' type="array"';
          $content = htmlspecialchars(join(',', $value));
        }
        else {
          $content = self::asXmlValue($value)."\n";
        }
      }
      elseif ($value instanceof ActiveRecord) {
        $content = $value->getAttributes();
      }
      else {
        if (is_bool($value)) {
          $xml .= ' type="boolean"';
          $value = (int) $value;
        }
        $content = htmlspecialchars($value);
      }
      $xml .= '>';
      $xml .= $content;
      $xml .= '</'.$key.'>';
    }
    return $xml;
  }
  
  public static function recursiveDiff($arr1=array(), $arr2=array()) { // ajouté par sofiane
    $ret_arr = array();
    foreach ($arr1 as $key => $value) {
      if (isset($arr2[$key])) {
        if (is_array($value)) {
          if (count($value) == 0) continue;
          $diff = self::recursiveDiff($value, $arr2[$key]);
          if (count($diff)) {
            $ret_arr[$key] = $diff;
          }
        }
        elseif ($value != $arr2[$key]) {
          $ret_arr[$key] = $arr2[$key];
        }
        unset($arr2[$key]);
      }
      else {
        if (is_array($value) && count($value) == 0) continue;
        $ret_arr[$key] = null;
      }
    }
    foreach ($arr2 as $key => $value) {
      if ($value) $ret_arr[$key] = $value;
    }
    return $ret_arr;
  }

}