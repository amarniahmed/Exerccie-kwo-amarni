<?php

// SELECT DISTINCT REVERSE(LEFT(REVERSE(email),LOCATE('@',REVERSE(email)) - 1)) AS domain, count(*) as n FROM utilisateurs group by domain ORDER BY n desc limit 100;

class Valid
{

  public static function isAlphanum(&$str, $strict_flag=false) {
    $regexp = ($strict_flag === true) ? '0-9|a-z' : '0-9|a-z|\-|_';
    return (preg_match('/^['.$regexp.']+$/iu', $str) === 1);
  }

  public static function isWord(&$s) {
    return preg_match('/^(\w+)$/u', $s);
  }

  public static function isNum(&$s) {
    return preg_match('/^(\d+)$/u', $s);
  } 

  public static function isFloat(&$s) {
    // alek
    return preg_match('/^-?(\d+)(\.?(\d+))?$/u', $s);
  } 

  public static function isRib(&$hash) {
    $bank = trim($hash['banque']);
    $office = trim($hash['guichet']);
    $account = trim($hash['compte']);
    $key = trim($hash['clef']);

    $chars = array('/[AJ]/', '/[BKS]/', '/[CLT]/', '/[DMU]/', '/[ENV]/',
                   '/[FOW]/', '/[GPX]/', '/[HQY]/', '/[IRZ]/');
    $values = array('1', '2', '3', '4', '5', '6', '7', '8', '9');
    
    $bank = preg_replace('/[^0-9]/u', '', $bank);
    $office = preg_replace('/[^0-9]/u', '', $office);
    $account = preg_replace($chars, $values, $account);
    
    if (strlen($bank) != 5) return false;
    if (strlen($office) != 5) return false;
    if (strlen($account) > 11) return false;
    if (strlen($key) != 2 || $key == '00') return false;
    
    $l = $bank.$office.str_pad($account, 11, '0', STR_PAD_LEFT).$key.'0';
    $keyChk = 0;
    for ($i = 0; $i < 24; $i += 4) {
      $keyChk = ($keyChk * 9 + substr($l, $i, 4)) % 97;
    }
    return !$keyChk;
  }
  
  public static function isUrl(&$url) {
    return filter_var($url, 
                      FILTER_VALIDATE_URL, 
                      array(FILTER_FLAG_SCHEME_REQUIRED, FILTER_FLAG_HOST_REQUIRED)) !== false;
  }

  public static function isSiren(&$siren) {
    $siren = str_replace(array(' ', '.', '-', '_'), 
                         array('', '', '', ''), 
                         $siren);
    if (!preg_match('/^(\d)(\d)(\d)(\d)(\d)(\d)(\d)(\d)(\d)$/u', 
                    $siren,	$matches)) {
      return false;
    }
    $matches[2] *= 2;
    $matches[4] *= 2;
    $matches[6] *= 2;
    $matches[8] *= 2;
    $sum = 0;
    
    for ($i = 1; $i < count($matches); $i++) {
      if ($matches[$i] > 9) {
	$a = (int) substr($matches[$i], 0, 1);
	$b = (int) substr($matches[$i], 1, 1);
	$matches[$i] = $a + $b;
      }
      $sum += $matches[$i];
    }
    return ($sum % 10) == 0;
  }

  public static function isSiret(&$siret) {
    $siret = str_replace(' ', '', $siret);
    if (((int) $siret) == 0) { // Hugues : add fix, refuses '00000000000000' SIRET
      return false;
    }
    if (!preg_match('/^(\d)(\d)(\d)(\d)(\d)(\d)(\d)(\d)(\d)(\d)(\d)(\d)(\d)(\d)$/u', 
                    $siret, $matches)) {
      return false;
    } 
    else {
      $matches_str = implode('', array_slice($matches, 1, 9));
      if (!self::isSiren($matches_str)) {
        //if (!self::isSiren(implode('', array_slice($matches, 1, 9)))) { // edit hug
	return false;
      }
    }
    $matches[1] *= 2;
    $matches[3] *= 2;
    $matches[5] *= 2;
    $matches[7] *= 2;
    $matches[9] *= 2;
    $matches[11] *= 2;
    $matches[13] *= 2;
    $sum = 0;
    
    for ($i = 1; $i < count($matches); $i++) {
      if ($matches[$i] > 9) {
	$a = (int) substr($matches[$i], 0, 1);
	$b = (int) substr($matches[$i], 1, 1);
	$matches[$i] = $a + $b;
      }
      $sum += $matches[$i];
    }
    return ($sum % 10) === 0;
  }

  public static function isIBan(&$iban, $country=null) {
    $check = array('AD' => array('length' => 24, 'regex' => '[0-9]{8}[A-Z0-9]{12}'),
                   'AT' => array('length' => 20, 'regex' => '[0-9]{5}[0-9]{11}'),
                   'BA' => array('length' => 20, 'regex' => '[0-9]{6}[0-9]{10}'),
                   'BE' => array('length' => 16, 'regex' => '[0-9]{3}[0-9]{9}'),
                   'BG' => array('length' => 22, 'regex' => '[A-Z]{4}[0-9]{4}[0-9]{2}[A-Z0-9]{8}'),
                   'CH' => array('length' => 21, 'regex' => '[0-9]{5}[A-Z0-9]{12}'),
                   'CS' => array('length' => 22, 'regex' => '[0-9]{3}[0-9]{15}'),
                   'CY' => array('length' => 28, 'regex' => '[0-9]{8}[A-Z0-9]{16}'),
                   'CZ' => array('length' => 24, 'regex' => '[0-9]{4}[0-9]{16}'),
                   'DE' => array('length' => 22, 'regex' => '[0-9]{8}[0-9]{10}'),
                   'DK' => array('length' => 18, 'regex' => '[0-9]{4}[0-9]{10}'),
                   'EE' => array('length' => 20, 'regex' => '[0-9]{4}[0-9]{12}'),
                   'ES' => array('length' => 24, 'regex' => '[0-9]{8}[0-9]{12}'),
                   'FR' => array('length' => 27, 'regex' => '[0-9]{10}[A-Z0-9]{13}'),
                   'FI' => array('length' => 18, 'regex' => '[0-9]{6}[0-9]{8}'),
                   'GB' => array('length' => 22, 'regex' => '[A-Z]{4}[0-9]{14}'),
                   'GI' => array('length' => 23, 'regex' => '[A-Z]{4}[A-Z0-9]{15}'),
                   'GR' => array('length' => 27, 'regex' => '[0-9]{7}[A-Z0-9]{16}'),
                   'HR' => array('length' => 21, 'regex' => '[0-9]{7}[0-9]{10}'),
                   'HU' => array('length' => 28, 'regex' => '[0-9]{7}[0-9]{1}[0-9]{15}[0-9]{1}'),
                   'IE' => array('length' => 22, 'regex' => '[A-Z0-9]{4}[0-9]{6}[0-9]{8}'),
                   'IS' => array('length' => 26, 'regex' => '[0-9]{4}[0-9]{18}'),
                   'IT' => array('length' => 27, 'regex' => '[A-Z]{1}[0-9]{10}[A-Z0-9]{12}'),
                   'LI' => array('length' => 21, 'regex' => '[0-9]{5}[A-Z0-9]{12}'),
                   'LU' => array('length' => 20, 'regex' => '[0-9]{3}[A-Z0-9]{13}'),
                   'LT' => array('length' => 20, 'regex' => '[0-9]{5}[0-9]{11}'),
                   'LV' => array('length' => 21, 'regex' => '[A-Z]{4}[A-Z0-9]{13}'),
                   'MK' => array('length' => 19, 'regex' => '[A-Z]{3}[A-Z0-9]{10}[0-9]{2}'),
                   'MT' => array('length' => 31, 'regex' => '[A-Z]{4}[0-9]{5}[A-Z0-9]{18}'),
                   'NL' => array('length' => 18, 'regex' => '[A-Z]{4}[0-9]{10}'),
                   'NO' => array('length' => 15, 'regex' => '[0-9]{4}[0-9]{7}'),
                   'PL' => array('length' => 28, 'regex' => '[0-9]{8}[0-9]{16}'),
                   'PT' => array('length' => 25, 'regex' => '[0-9]{8}[0-9]{13}'),
                   'RO' => array('length' => 24, 'regex' => '[A-Z]{4}[A-Z0-9]{16}'),
                   'SE' => array('length' => 24, 'regex' => '[0-9]{3}[0-9]{17}'),
                   'SI' => array('length' => 19, 'regex' => '[0-9]{5}[0-9]{8}[0-9]{2}'),
                   'SK' => array('length' => 24, 'regex' => '[0-9]{4}[0-9]{16}'),
                   'TN' => array('length' => 24, 'regex' => '[0-9]{5}[0-9]{15}'),
                   'TR' => array('length' => 26, 'regex' => '[0-9]{5}[A-Z0-9]{17}'));

    $iban = strtoupper(preg_replace('/[\W]+/u', '', $iban));
    
    if ($country !== null) {
      $country = strtoupper($country);
      if ($country != substr($iban, 0, 2)) {
        return false;
      }
    } 
    else {
      $country = strtoupper(substr($iban,0,2));
    }
    if (strlen($iban) != $check[$country]['length'] || 
        !preg_match('/^'.strtoupper($country).'[0-9]{2}'.$check[$country]['regex'].'$/u', $iban)) {
      return false;
    }
    $iban_replace_chars = range('A', 'Z');
    foreach (range(10, 35) as $tempvalue) {
      $iban_replace_values[] = strval($tempvalue);
    }
    $tempiban = substr($iban, 4).substr($iban, 0, 4);
    $tempiban = str_replace($iban_replace_chars, $iban_replace_values, $tempiban);
    $tempcheckvalue = intval(substr($tempiban, 0, 1));
    for ($strcounter = 1; $strcounter < strlen($tempiban); $strcounter++) {
      $tempcheckvalue *= 10;
      $tempcheckvalue += intval(substr($tempiban, $strcounter,1));
      $tempcheckvalue %= 97;
    }
    return $tempcheckvalue != 1 ? false : $iban;
  }

  public static function isBicCode(&$code, $country = null) {
    $code = strtoupper(preg_replace('/[\W]+/u', '', $code));
    if ($country !== null) {
      $country = strtoupper($country);
      if ($country != $code{4}.$code{5}) {
        return false;
      }
    }
    if (strlen($code) === 8 || strlen($code) === 11) {
      return $code;
    }
    return false;
  }

  /* ref:  http://ec.europa.eu/taxation_customs/vies/faqvies.do#item11 */
  public static function isVat(&$code, $country=null) {
    $format = array('BE' => '0[[:digit:]]{9}',
                    'GB' => '[[:digit:]]{9}|[[:digit:]]{12}|GD[[:digit:]]{3}|HA[[:digit:]]{3}',
                    'FR' => '[[:alnum:]]{2}[[:digit:]]{9}');
    $code = strtoupper(preg_replace('/[\W]+/u', '', $code));
    if ($country !== null) {
      $country = strtoupper($country);
      if ($country != substr($code, 0, 2)) {
        return false;
      }
    } else {
      $country = strtoupper(substr($code, 0, 2));
    }
    return preg_match('/^'.$country.'('.$format[$country].')$/u', $code) > 0;
  }

}
