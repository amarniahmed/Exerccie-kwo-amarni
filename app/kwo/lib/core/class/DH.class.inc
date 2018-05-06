<?php

final class DH
{
  const MIN_YEAR = 1600;
  const DATE_MYSQL = 'Y-m-d H:i:s';
  const DATE_NULL = '0000-00-00 00:00:00';

  public static function asHash($date) {
    if (strlen($date) < 6 || substr($date, 0, 2) === '00') {
      return null;
    }
    $h = array();
    if ($date[2] === '/' || $date[1] === '/' ) {
      list ($h['date'], $h['time']) = explode(' ', $date);
      list ($h['day'], $h['month'], $h['year']) = explode('/', $h['date']);
      list ($h['hour'], $h['min'], $h['sec']) = explode(':', $h['time']);
    }
    elseif ($date[4] === '-') {
      list ($h['date'], $h['time']) = explode(' ', $date);
      list ($h['year'], $h['month'], $h['day']) = explode('-', $h['date']);
      list ($h['hour'], $h['min'], $h['sec']) = explode(':', $h['time']);
    }
    elseif (strlen($date) == 10 && is_numeric($date)) {
      $date = (int) $date;
      $h['date'] = date('Y-m-d', $date);
      $h['year'] = date('Y', $date);
      $h['month'] = date('m', $date);
      $h['day'] = date('d', $date);
      $h['time'] = date('H:i:s', $date);
      $h['hour'] = date('H', $date);
      $h['min'] = date('i', $date);
      $h['sec'] = date('s', $date);
    }
    if ($h['year'] < self::MIN_YEAR) {
      S('log')->trace('bad input ('.$date.')', __METHOD__);
      return false;
    }
    $h['ts'] = mktime((int) $h['hour'], (int) $h['min'], (int) $h['sec'],
                      (int) $h['month'], (int) $h['day'], (int) $h['year']);
    $h['wkday'] = date('N', $h['ts']);
    return $h;
  }

  public static function asTimestamp($date) {
    if (strlen($date) < 6 || substr($date, 0, 2) === '00') {
      S('log')->trace('bad input ('.$date.')', __METHOD__);
      return null;
    }
    $h = self::asHash($date);
    return $h['ts'];
  }

  public static function isBetween($from, $to, $date=null) {
    if (self::isNull($from) && self::isNull($to)) return true;
    $date = $date === null ? time() : self::asTimestamp($date);
    if (!self::isNull($from) && $date < self::asTimestamp($from)) {
      return false;
    }
    if (!self::isNull($to) && $date > self::asTimestamp($to)) {
      return false;
    }
    return true;
  }

  public static function isValid($date) {
    $h = self::asHash($date);
    if (!is_array($h)) return false;
    return checkdate($h['month'], $h['day'], $h['year']);
  }

  public static function toDb($date=null) {
    $time = null;
    $date = trim($date);
    if (self::isNull($date)) {
      return '0000-00-00';
    }
    if (is_numeric($date) && $date > 100000) {
      return date(self::DATE_MYSQL, $date);
    }
    if (strpos($date, ':') !== false) {
      list ($date, $time) = explode(' ', $date);
    }
    if (strpos($date, '-') !== false) {
      list ($year, $month, $day) = explode('-', $date);
    }
    else {
      list ($day, $month, $year) = explode('/', $date);
    }
    if ($year <= self::MIN_YEAR || $month <= 0 || $day <= 0) {
      return '0000-00-00';
    }
    $ret = $year.'-'.$month.'-'.$day;
    if ($time != null) $ret .= ' '.$time;
    return $ret;
  }

/*  public static function nextday($date) {
    $date = self::toDb($date);
    if ($date === '0000-00-00') return '0000-00-00';
  } */

  public static function when($diff=0) {
    return date(self::DATE_MYSQL, (time() + $diff));
  }

  public static function now() {
    return date(self::DATE_MYSQL);
  }

  private static $today = null;

  public static function today() {
    if (is_null(self::$today)) {
      self::$today = date('Y-m-d');
    }
    return self::$today;
  }

  public static function yesterday() {
    $ts = mktime(0, 0, 0, date('m'), date('d') - 1, date('Y'));
    return date('Y-m-d', $ts);
  }

  public static function isToday($h) {
    return (intval($h['day']) == intval(date('d')) &&
            intval($h['month']) == intval(date('m')) &&
            intval($h['year']) == intval(date('Y')));
  }

  public static function isYesterday($h) {
    return (intval($h['day']) == intval(date('d') - 1) &&
            intval($h['month']) == intval(date('m')) &&
            intval($h['year']) == intval(date('Y')));
  }

  public static function isNull($date) {
    $date = (string) $date;
    return empty($date) ||
           (is_numeric($date) && $date == 0) ||
           substr($date, 0, 2) === '00' ||
           !in_array($date[0], array('0', '1', '2', '3'));
  }

  public static function sec2min($sec) {
    return floor($sec / 60) . 'min ' . ($sec % 60) . 'sec';
  }

  public static function min2hour($min) {
    return floor($min / 60) . 'h' . str_pad(($min % 60), 2, '0', STR_PAD_LEFT);
  }

  public static function asTextual($date, $locale=null) {
    $locale = $locale ?: I18N::LOCALE_FR;
    $h = self::asHash($date);
    $ts = mktime(0, 0, 0, $h['month'], $h['day'], $h['year']);
    return join(' ',
                array(I18N::$days[$locale]['long'][date('N', $ts)],
                      date('j', $ts),
                      I18N::$months[$locale]['long'][date('n', $ts)],
                      $h['year']));
  }

  private static $holidays = array();

  public static function isHoliday($date=null) {
    $date = $date ?: self::today();
    list ($year, $month, $day) = explode('-', $date);
    if (empty(self::$holidays[$year])) {
      self::$holidays[$year] = array();
      $holidays = array('01-01', '05-01', '05-08', '07-14', '08-15', '11-01', '11-11', '12-25');
      foreach ($holidays as $hday) {
        self::$holidays[$year][] = $year.'-'.$hday;
      }
      $pacques = self::pacques($year);
      self::$holidays[$year][] = $pacques;
      $h = self::asHash($pacques);
      $ascension = mktime(0, 0, 0, $h['month'], $h['day'] + 38, $h['year']);
      $ascension = date('Y-m-d', $ascension);
      self::$holidays[$year][] = $ascension;
      $pentecote = mktime(0, 0, 0, $h['month'], $h['day'] + 49, $h['year']);
      $pentecote = date('Y-m-d', $pentecote);
      self::$holidays[$year][] = $pentecote;
/*      S('log')->debug(array('pacques' => $pacques,
                            'ascension' => $ascension,
                            'pentecote' => $pentecote));*/
    }
    return in_array($date, self::$holidays[$year]);
  }

  public static function pacques($year) {
    $Jourj = 1; // lundi
//    $year = $year ?: date('Y');
    $G = $year % 19;
    $C = floor($year / 100);
    $C_4 = floor($C / 4);
    $E = floor((8 * $C + 13) / 25);
    $H = (19 * $G + $C - $C_4 - $E + 15) % 30;
    if ($H == 29) {
      $H = 28;
    }
    elseif ($H == 28 && $G > 10) {
      $H = 27;
    }
    $K = floor($H / 28);
    $P = floor(29 / ($H + 1));
    $Q = floor((21 - $G) / 11);
    $I = ($K * $P * $Q - 1) * $K + $H;
    $B = floor($year / 4) + $year;
    $J1 = $B + $I + 2 + $C_4 - $C;
    $J2 = $J1 % 7;
    $R = 28 + $I - $J2;
    $mois = $R > 30 ? 4 : 3;
    $Jour = $mois == 3 ? $R : $R - 31;

    $ts = mktime(0, 0, 0, $mois, $Jour + $Jourj, $year);
    return date('Y-m-d', $ts);
  }

}
