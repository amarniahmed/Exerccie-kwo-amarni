<?php

/*- 12.345,67 (German)
- 12,345.67 (English)
- 12 345,67 (French)
- 12'345,67 (Swiss)

http://fr.wikipedia.org/wiki/Liste_des_codes_ISO_639-1

*/

class I18N {

  const LOCALE_FR = 1;        // 1 :
  const LOCALE_EN = 2;        // 2 :
  const LOCALE_ES = 4;        // 3 : ESPAGNE
  const LOCALE_IT = 8;        // 4 : ITALIE
  const LOCALE_PT = 16;       // 5 : PORTUGAL
  const LOCALE_DE = 32;       // 6 : ALLEMAGNE
  const LOCALE_NL = 64;       // 7 : NETHERLANDS
  const LOCALE_DK = 128;      // 8 : DANEMARK
  const LOCALE_FI = 256;      // 9 : FINLANDE
  const LOCALE_GR = 512;      // 10 : GRÈCE
  const LOCALE_SE = 1024;     // 11 : SUÈDE
  const LOCALE_EE = 2048;     // 12 : ESTONIE
  const LOCALE_HU = 4096;     // 13 : HONGRIE
  const LOCALE_MT = 8192;     // 14 : MALTE
  const LOCALE_LV = 16384;    // 15 : LETTONIE
  const LOCALE_LT = 32768;    // 16 : LITUANIE
  const LOCALE_PL = 65536;    // 17 : POLOGNE
  const LOCALE_CZ = 131072;   // 18 : TCHÈQUE, RÉP
  const LOCALE_SK = 262144;   // 19 : SLOVAQUIE
  const LOCALE_SI = 524288;   // 20 : SLOVÉNIE
  const LOCALE_BG = 1048576;  // 21 : BULGARIE
  const LOCALE_RO = 2097152;  // 22 : ROUMANIE
  const LOCALE_JA = 4194304;  // 23 : JAPON
  const LOCALE_CN = 8388608;  // 24 : CHINE
  const LOCALE_HK = 16777216; // 25 : HONGKONG
  const LOCALE_RU = 33554432; // 26 : RUSSE

  const COUNTRY_FR = 250;

  private static $locales = array(self::LOCALE_FR => 'fr',
                                  self::LOCALE_EN => 'en',
                                  self::LOCALE_ES => 'es',
                                  self::LOCALE_IT => 'it',
                                  self::LOCALE_PT => 'pt',
                                  self::LOCALE_DE => 'de',
                                  self::LOCALE_NL => 'nl',
                                  self::LOCALE_DK => 'dk',
                                  self::LOCALE_FI => 'fi',
                                  self::LOCALE_GR => 'gr',
                                  self::LOCALE_SE => 'se',
                                  self::LOCALE_EE => 'ee',
                                  self::LOCALE_HU => 'hu',
                                  self::LOCALE_MT => 'mt',
                                  self::LOCALE_LV => 'lv',
                                  self::LOCALE_LT => 'lt',
                                  self::LOCALE_PL => 'pl',
                                  self::LOCALE_CZ => 'cz',
                                  self::LOCALE_SK => 'sk',
                                  self::LOCALE_SI => 'si',
                                  self::LOCALE_BG => 'bg',
                                  self::LOCALE_RO => 'ro',
                                  self::LOCALE_JA => 'ja',
                                  self::LOCALE_CN => 'cn',
                                  self::LOCALE_HK => 'hk',
                                  self::LOCALE_RU => 'ru');

  public static $langs = array(self::LOCALE_FR => 'fr',
                               self::LOCALE_EN => 'en',
                               self::LOCALE_ES => 'es',
                               self::LOCALE_IT => 'it',
                               self::LOCALE_PT => 'pt',
                               self::LOCALE_DE => 'de',
                               self::LOCALE_NL => 'nl',
                               self::LOCALE_DK => 'dk',
                               self::LOCALE_FI => 'fi',
                               self::LOCALE_GR => 'gr',
                               self::LOCALE_SE => 'se',
                               self::LOCALE_EE => 'ee',
                               self::LOCALE_HU => 'hu',
                               self::LOCALE_MT => 'mt',
                               self::LOCALE_LV => 'lv',
                               self::LOCALE_LT => 'lt',
                               self::LOCALE_PL => 'pl',
                               self::LOCALE_CZ => 'cz',
                               self::LOCALE_SK => 'sk',
                               self::LOCALE_SI => 'si',
                               self::LOCALE_BG => 'bg',
                               self::LOCALE_RO => 'ro',
                               self::LOCALE_JA => 'ja',
                               self::LOCALE_CN => 'cn',
                               self::LOCALE_HK => 'hk',
                               self::LOCALE_RU => 'ru');

  public static $locale_labels = array(self::LOCALE_FR => 'Français',
                                       self::LOCALE_EN => 'English',
                                       self::LOCALE_ES => 'Español',
                                       self::LOCALE_IT => 'Italiano',
                                       self::LOCALE_PT => 'Português',
                                       self::LOCALE_DE => 'Deutsch',
                                       self::LOCALE_NL => 'Nederlands',
                                       self::LOCALE_DK => 'Danish',
                                       self::LOCALE_FI => 'Finnish',
                                       self::LOCALE_GR => 'Greek',
                                       self::LOCALE_SE => 'Swedish',
                                       self::LOCALE_EE => 'Estonian',
                                       self::LOCALE_HU => 'Hungarian',
                                       self::LOCALE_MT => 'Maltese',
                                       self::LOCALE_LV => 'Latvian',
                                       self::LOCALE_LT => 'Lithuanian',
                                       self::LOCALE_PL => 'Polish',
                                       self::LOCALE_CZ => 'Czech',
                                       self::LOCALE_SK => 'Slovak',
                                       self::LOCALE_SI => 'Slovenian',
                                       self::LOCALE_BG => 'Bulgarian',
                                       self::LOCALE_RO => 'Romanian',
                                       self::LOCALE_JA => 'Japonais',
                                       self::LOCALE_PT => 'Portuguès',
                                       self::LOCALE_CN => 'Chinese',
                                       self::LOCALE_HK => 'Hong Kong',
                                       self::LOCALE_RU => 'Russian');

  public static $metas = array(self::LOCALE_FR => 'fr_FR',
                               self::LOCALE_EN => 'en_EN',
                               self::LOCALE_ES => 'es_ES',
                               self::LOCALE_IT => 'it_IT',
                               self::LOCALE_PT => 'pt_PT',
                               self::LOCALE_DE => 'de_DE',
                               self::LOCALE_NL => 'nl',
                               self::LOCALE_DK => 'dk',
                               self::LOCALE_FI => 'fi_FI',
                               self::LOCALE_GR => 'gr',
                               self::LOCALE_SE => 'se',
                               self::LOCALE_EE => 'ee',
                               self::LOCALE_HU => 'hu_HU',
                               self::LOCALE_MT => 'mt_MT',
                               self::LOCALE_LV => 'lv_LV',
                               self::LOCALE_LT => 'lt_LT',
                               self::LOCALE_PL => 'pl_PL',
                               self::LOCALE_CZ => 'cz',
                               self::LOCALE_SK => 'sk_SK',
                               self::LOCALE_SI => 'si_LK',
                               self::LOCALE_BG => 'bg_BG',
                               self::LOCALE_RO => 'ro_RO',
                               self::LOCALE_JA => 'ja_JP',
                               self::LOCALE_CN => 'cn',
                               self::LOCALE_HK => 'hk',
                               self::LOCALE_RU => 'ru_RU');

  public static $months = array(self::LOCALE_FR => array('short' => array(1 => 'janv', 2 => 'fév', 3 => 'mars', 4 => 'avr', 5 => 'mai', 6 => 'juin',
                                                                        7 => 'juil', 8 => 'aout', 9 => 'sept', 10 => 'oct', 11 => 'nov' , 12 => 'déc'),

                                                         'long' => array(1 => 'janvier', 2 => 'février', 3 => 'mars', 4 => 'avril', 5 => 'mai', 6 => 'juin',
                                                                         7 => 'juillet', 8 => 'août', 9 => 'septembre', 10 => 'octobre', 11 => 'novembre', 12 => 'décembre')),

                                self::LOCALE_EN => array('short' => array(1 => 'jan', 2 => 'feb', 3 => 'mar', 4 => 'apr', 5 => 'may', 6 => 'jun',
                                                                          7 => 'jul', 8 => 'aug', 9 => 'sep', 10 => 'oct', 11 => 'nov' , 12 => 'dec'),

                                                         'long' => array(1 => 'january', 2 => 'february', 3 => 'march', 4 => 'april', 5 => 'may', 6 => 'june',
                                                                         7 => 'july', 8 => 'august', 9 => 'september', 10 => 'october', 11 => 'november', 12 =>'december')));

  public static $days = array(self::LOCALE_FR => array('short' => array(1 => 'lun', 2 => 'mar', 3 => 'mer', 4 => 'jeu', 5 => 'ven', 6 => 'sam', 7 => 'dim'),
                                                       'long' => array(1 => 'lundi', 2 => 'mardi', 3 => 'mercredi', 4 => 'jeudi', 5 => 'vendredi', 6 => 'samedi', 7 => 'dimanche')),

                              self::LOCALE_EN => array('short' => array(1 => 'mon', 2 => 'tue', 3 => 'wed', 4 => 'thu', 5 => 'fri', 6 => 'sat', 7 => 'sun'),
                                                       'long' => array(1 => 'monday', 2 => 'tuesday', 3 => 'wednesday', 4 => 'thursday', 5 => 'friday', 6 => 'saturday', 7 => 'sunday')));


  public static function locale2num($locale) {
    if ($locale == 'fr') return 1;
    if (is_numeric($locale)) return (int) $locale;
    $locales = array_flip(self::$locales);
    if (!isset($locales[$locale])) {
      S('log')->error('invalid locale ['.$locale.']', __METHOD__);
    }
    return $locales[$locale];
  }

  public static function locale2code($locale) {
    if ($locale == 1) return 'fr';
    if (!is_numeric($locale)) return $locale;
    $locale = (int) $locale;
    if (!isset(self::$locales[$locale])) {
      S('log')->error('invalid locale ('.$locale.')', __METHOD__);
    }
    return self::$locales[$locale];
  }

  public static function locale2label($locale) {
    $locale = self::locale2num($locale);
    return self::$locale_labels[$locale];
  }

  public static function locales() {
    $locales = P('back.locales');
    if ($locales === null) {
      $locales = P('app.locales');
    }
    $locales = (array) $locales;
    $h = array();
    foreach ($locales as $locale) {
      $h[$locale] = self::$locales[$locale];
    }
    return $h;
  }

  public static function day($num, $locale=self::LOCALE_FR, $size='long') {
    return self::$days[$locale][$size][$num];
  }

  public static function month($num, $locale=self::LOCALE_FR, $size='long') {
    return self::$months[$locale][$size][$num];
  }

  public static function getCountries($locale=self::LOCALE_FR) {
    $lang = $locale === self::LOCALE_FR ? '_fr' : '_en';
    return S('dbo')->asHash('SELECT id, name#lang#'
                            .' FROM '.T('country')
                            .' WHERE status=1'
                            .' ORDER BY name#lang#',
                            array('#lang#' => $lang));
  }

}